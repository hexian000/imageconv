#include "fft.hpp"

#include <fftw3.h>

#include <memory>
#include <mutex>
#include <thread>

static_assert(sizeof(std::complex<double>) == sizeof(fftw_complex),
    "incompatible std::complex implementation");

using namespace std;

struct fft_private {
    fftw_plan plan;

    explicit fft_private(
        const size_t width, const size_t height, bool backward) {
        const auto sign = backward ? FFTW_BACKWARD : FFTW_FORWARD;
        const auto buf = fftw_alloc_complex(width * height);
        plan = ::fftw_plan_dft_2d(static_cast<int>(height),
            static_cast<int>(width), buf, buf, sign, FFTW_ESTIMATE);
        fftw_free(buf);
    }

    ~fft_private() { fftw_destroy_plan(plan); }
};

fft::fft(const size_t width, const size_t height, bool backward)
    : p(make_unique<fft_private>(width, height, backward)) {}

fft::~fft() = default;

void fft::compute(std::shared_ptr<std::complex<double>> buf) {
    const auto b = reinterpret_cast<fftw_complex *>(buf.get());
    fftw_execute_dft(p->plan, b, b);
}

shared_ptr<complex<double>> fft::new_buffer(size_t n) {
    static const auto deleter = [](auto p) { ::fftw_free(p); };
    const auto p = reinterpret_cast<complex<double> *>(::fftw_alloc_complex(n));
    return shared_ptr<complex<double>>{p, deleter};
}

void fft::cleanup() { fftw_cleanup(); }
