#include "methods.hpp"

#include <algorithm>
#include <complex>
#include <tuple>

namespace {
template<typename T>
inline T abs(T x) {
    return x < 0 ? -x : x;
}
} // namespace

void methods::copy(
    std::complex<double> *dst, std::complex<double> *src, const int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

void methods::multiply(
    std::complex<double> *a, const std::complex<double> *k, const int n) {
    for (int i = 0; i < n; i++) {
        a[i] *= k[i];
    }
}

void methods::lowpass(std::complex<double> *a, const int w, const int h,
    const int w1, const int h1) {
    for (int y = 0; y < h1; y++) {
        for (int x = w1; x < w - w1; x++) {
            a[y * w + x] = 0;
        }
    }
    for (int y = h1; y < h - h1; y++) {
        for (int x = 0; x < w; x++) {
            a[y * w + x] = 0;
        }
    }
    for (int y = h - h1; y < h; y++) {
        for (int x = w1; x < w - w1; x++) {
            a[y * w + x] = 0;
        }
    }
}

void methods::fftshift(std::complex<double> *a, std::complex<double> *t,
    const int w, const int h) {
    const auto w2 = w / 2;
    const auto h2 = h / 2;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t[y * w + x] = a[y * w + (x + w2) % w];
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            a[y * w + x] = t[(y + h2) % h * w + x];
        }
    }
}

void methods::ifftshift(std::complex<double> *a, std::complex<double> *t,
    const int w, const int h) {
    const auto w2 = w - w / 2;
    const auto h2 = h - h / 2;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t[y * w + x] = a[y * w + (x + w2) % w];
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            a[y * w + x] = t[(y + h2) % h * w + x];
        }
    }
}

namespace {
inline std::tuple<double, double, double> palette(const double v) {
    if (!std::isfinite(v)) { return {0, 0, 0}; }
    const auto r = std::clamp(v * 400, 0.0, 1.0);
    const auto g = std::clamp(v * 20, 0.0, 1.0);
    const auto b = std::clamp(v, 0.0, 1.0);
    return {r, g, b};
}
} // namespace

void methods::spectrum(std::complex<double> *dst_r, std::complex<double> *dst_g,
    std::complex<double> *dst_b, const std::complex<double> *src_r,
    const std::complex<double> *src_g, const std::complex<double> *src_b,
    const int width, const int height) {
    using namespace std;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const auto i = y * width + x;
            const auto v = 0.2126 * norm(src_r[i]) + 0.7152 * norm(src_g[i]) +
                           0.0722 * norm(src_b[i]);
            dst_r[i] = v;
            dst_g[i] = v;
        }
    }
    std::sort(dst_g, dst_g + (width * height),
        [](std::complex<double> a, std::complex<double> b) {
            return a.real() < b.real();
        });
    const auto v0 = dst_g[static_cast<int>(0.05 * (width * height - 1))].real();
    const auto k =
        1.0 / dst_g[static_cast<int>(0.95 * (width * height - 1))].real();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const auto i = y * width + x;
            const auto v = dst_r[i].real();
            tie(dst_r[i], dst_g[i], dst_b[i]) = palette(k * (v - v0));
        }
    }
}

void methods::downsample2x(std::complex<double> *dst, std::complex<double> *src,
    const int w0, const int h0) {
    using namespace std;
    const auto w1 = w0 / 2;
    const auto h1 = h0 / 2;
    const auto w2 = w1 / 2;
    const auto h2 = h1 / 2;
    for (int y1 = 0; y1 < h2; y1++) {
        const auto y0 = y1;
        for (int x1 = 0; x1 < w2; x1++) {
            const auto x0 = x1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
        for (int x1 = w2; x1 < w1; x1++) {
            const auto x0 = w0 + x1 - w1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
    }
    for (int y1 = h2; y1 < h1; y1++) {
        const auto y0 = h0 + y1 - h1;
        for (int x1 = 0; x1 < w2; x1++) {
            const auto x0 = x1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
        for (int x1 = w2; x1 < w1; x1++) {
            const auto x0 = w0 + x1 - w1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
    }
}

void methods::upsample2x(std::complex<double> *dst, std::complex<double> *src,
    const int w0, const int h0) {
    const int w1 = w0 * 2, h1 = h0 * 2;
    const int w2 = w0 / 2, h2 = h0 / 2;
    using namespace std;
    for (int y1 = 0; y1 < h2; y1++) {
        const auto y0 = y1;
        for (int x1 = 0; x1 < w2; x1++) {
            const auto x0 = x1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
        for (int x1 = w2; x1 < w0 - w2; x1++) {
            dst[y1 * w1 + x1] = 0;
        }
        for (int x1 = w1 - w2; x1 < w1; x1++) {
            const auto x0 = x1 - w0;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
    }
    for (int y1 = h2; y1 < h1 - h2; y1++) {
        for (int x1 = 0; x1 < w1; x1++) {
            dst[y1 * w1 + x1] = 0;
        }
    }
    for (int y1 = h1 - h2; y1 < h1; y1++) {
        const auto y0 = y1 - h0;
        for (int x1 = 0; x1 < w2; x1++) {
            const auto x0 = x1;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
        for (int x1 = w2; x1 < w0 - w2; x1++) {
            dst[y1 * w1 + x1] = 0;
        }
        for (int x1 = w1 - w2; x1 < w1; x1++) {
            const auto x0 = x1 - w0;
            dst[y1 * w1 + x1] = src[y0 * w0 + x0];
        }
    }
}
