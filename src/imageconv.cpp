#include "imageconv.hpp"
#include "fft.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "methods.hpp"

#include <chrono>
#include <complex>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <tuple>

imageconv::imageconv(const options &op) { option = op; }

void imageconv::run() const {
    using namespace std;

    const auto &op = option;
    cout << op.input << " => " << op.output << endl;
    cout << "method: " << op.get_method_str() << endl;

    unsigned extend;
    size_t width, height; // image size
    size_t w, h;          // transform size
    shared_ptr<complex<double>> r, g, b;
    unique_ptr<image> input, output;

    {
        const auto begin = chrono::steady_clock::now();

        input = make_unique<image>(op.input);
        tie(width, height) = input->get_size();
        extend = min(static_cast<unsigned>(min(width, height)), op.extend);
        tie(w, h) = input->get_extended_size(extend);
        cout << "image size: " << width << "x" << height << '\n'
             << "transform size: " << w << "x" << h << " = " << w * h << '\n'
             << "estimated memory: "
             << ((w * h * sizeof(complex<double>) * 4 + width * height * 4) >>
                    20u)
             << " MiB" << endl;

        const auto end = chrono::steady_clock::now();
        cerr << "read ... "
             << chrono::duration_cast<chrono::milliseconds>(end - begin).count()
             << " ms" << endl;
    }

    {
        const auto begin = chrono::steady_clock::now();

        unique_ptr<fft> transform = make_unique<fft>(w, h);
        auto data = async([&input, extend, w, h] {
            auto r = fft::new_buffer(w * h);
            auto g = fft::new_buffer(w * h);
            auto b = fft::new_buffer(w * h);
            if (extend > 0) {
                input->load_extended(extend, r.get(), g.get(), b.get());
            } else {
                input->load(r.get(), g.get(), b.get());
            }
            input = nullptr;
            return make_tuple(r, g, b);
        });
        auto kernel = async([&transform, &op, w, h] {
            std::shared_ptr<std::complex<double>> k;
            switch (op.method) {
            case method_type::nop: {
                k = fft::new_buffer(w * h);
                kernel::identity(k.get(), w, h);
                transform->compute(k);
            } break;
            case method_type::gaussian: {
                k = fft::new_buffer(w * h);
                kernel::gaussian(k.get(), w, h, op.weight);
                transform->compute(k);
            } break;
            case method_type::spectrum: break;
            case method_type::downscale2x: {
                k = fft::new_buffer(w * h);
                kernel::mitchell(k.get(), w, h, 2.0);
                transform->compute(k);
            } break;
            case method_type::upscale2x: {
                k = fft::new_buffer(w * h);
                kernel::lanczos(k.get(), w, h, 2.0, 10.0);
                transform->compute(k);
            } break;
            }
            return k;
        });

        unique_ptr<fft> transform_inv;
        function<shared_ptr<complex<double>>(
            shared_ptr<complex<double>>, shared_ptr<complex<double>>)>
            compute;
        switch (op.method) {
        case method_type::nop:
        case method_type::gaussian: {
            transform_inv = make_unique<fft>(w, h, true);
            compute = [&transform, &transform_inv, w, h](
                          shared_ptr<complex<double>> c,
                          shared_ptr<complex<double>> k) {
                transform->compute(c);
                const auto n = w * h;
                methods::multiply(c.get(), k.get(), n);
                k = nullptr;
                transform_inv->compute(c);
                return c;
            };
        } break;
        case method_type::spectrum: {
            transform_inv = make_unique<fft>(w, h, true);
            compute = [&transform](shared_ptr<complex<double>> c,
                          shared_ptr<complex<double>>) {
                transform->compute(c);
                return c;
            };
        } break;
        case method_type::downscale2x: {
            transform_inv = make_unique<fft>(w / 2, h / 2, true);
            compute = [&transform, &transform_inv, w, h](
                          shared_ptr<complex<double>> c,
                          shared_ptr<complex<double>> k) {
                transform->compute(c);
                const auto n = w * h;
                methods::multiply(c.get(), k.get(), n);
                k = nullptr;
                auto dst_c = fft::new_buffer((w / 2) * (h / 2));
                methods::downsample2x(dst_c.get(), c.get(), w, h);
                c = nullptr;
                transform_inv->compute(dst_c);
                return dst_c;
            };
        } break;
        case method_type::upscale2x:
            transform_inv = make_unique<fft>(w * 2, h * 2, true);
            compute = [&transform, &transform_inv, w, h](
                          shared_ptr<complex<double>> c,
                          shared_ptr<complex<double>> k) {
                transform->compute(c);
                // const auto n = w * h;
                // methods::multiply(c.get(), k.get(), n);
                k = nullptr;
                auto dst_c = fft::new_buffer((w * 2) * (h * 2));
                methods::upsample2x(dst_c.get(), c.get(), w, h);
                c = nullptr;
                transform_inv->compute(dst_c);
                return dst_c;
            };
            break;
        }

        tie(r, g, b) = [&data, &kernel, &compute] {
            auto k = kernel.get();
            auto [in_r, in_g, in_b] = data.get();
            auto future_r = async(compute, in_r, k);
            auto future_g = async(compute, in_g, k);
            auto future_b = async(compute, in_b, k);
            return make_tuple(future_r.get(), future_g.get(), future_b.get());
        }();

        switch (op.method) {
        case method_type::nop:
        case method_type::gaussian:
            output = make_unique<image>(width, height);
            break;
        case method_type::spectrum:
            methods::spectrum(r.get(), g.get(), b.get(), r.get(), g.get(),
                b.get(), width, height);
            output = make_unique<image>(width, height);
            break;
        case method_type::downscale2x:
            output = make_unique<image>(width / 2, height / 2);
            extend /= 2;
            break;
        case method_type::upscale2x:
            output = make_unique<image>(width * 2, height * 2);
            extend *= 2;
            break;
        }

        if (extend > 0) {
            output->save_extended(extend, r.get(), g.get(), b.get());
        } else {
            output->save(r.get(), g.get(), b.get());
        }
        r = nullptr;
        g = nullptr;
        b = nullptr;
        transform = nullptr;
        transform_inv = nullptr;

        const auto end = chrono::steady_clock::now();
        cerr << "compute ... "
             << chrono::duration_cast<chrono::milliseconds>(end - begin).count()
             << " ms" << endl;
    }
    fft::cleanup();

    {
        const auto begin = chrono::steady_clock::now();
        output->write(op.output);
        const auto end = chrono::steady_clock::now();
        cerr << "save ... "
             << chrono::duration_cast<chrono::milliseconds>(end - begin).count()
             << " ms" << endl;
    }
}
