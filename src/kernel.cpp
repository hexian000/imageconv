#include "kernel.hpp"

#include <cmath>

namespace {
inline double sqr(double x) { return x * x; }

template<typename T>
inline T abs(T x) {
    return x < 0 ? -x : x;
}

inline int map_axis(const int v, const int max) {
    const auto max2 = max / 2;
    if (v < max2) { return v; }
    return v - max;
}

void normalize(
    std::complex<double> *kernel, const double sum, const int w, const int h) {
    const auto scale = w * h * sum;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            auto &v = kernel[y * w + x];
            v = v.real() / scale;
        }
    }
}
} // namespace

void kernel::identity(std::complex<double> *kernel, const int w, const int h) {
    const auto n = w * h;
    kernel[0] = 1.0 / n;
    for (int i = 1; i < n; i++) {
        kernel[i] = 0;
    }
}

void kernel::gaussian(std::complex<double> *kernel, const int w, const int h,
    const double weight) {
    using namespace std;
    const auto scale = 1.0 / (2.0 * M_PI * sqr(weight)) / (w * h);
    const auto constant = -2.0 * sqr(weight);
    auto sum = 0.0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const auto value =
                exp((sqr(map_axis(x, w)) + sqr(map_axis(y, h))) / constant) *
                scale;
            sum += value;
            kernel[y * w + x] = value;
        }
    }
    normalize(kernel, sum, w, h);
}

namespace {
inline double mitchell_(double x) {
    static constexpr auto B = 1.0 / 3.0;
    static constexpr auto C = 1.0 / 3.0;
    using namespace std;
    x = fabs(x);
    // clang-format off
        if (x < 1.0) {
            return (
                (6.0 - 2.0 * B) +
                ((-18.0 + 12.0 * B + 6.0 * C) +
                (12.0 - 9.0 * B - 6.0 * C) * x) *
                (x * x) ) / 6.0;
        }
        if (x < 2.0) {
            return (
                (8.0 * B + 24.0 * C) +
                ((-12.0 * B - 48.0 * C) +
                ((6.0 * B + 30.0 * C) -
                (B + 6.0 * C) * x) * x) * x ) / 6.0;
        }
    // clang-format on
    return 0.0;
}
} // namespace

void kernel::mitchell(std::complex<double> *kernel, const int w, const int h,
    const double scale) {
    using namespace std;
    auto sum = 0.0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            auto &value = kernel[y * w + x];
            const auto d = hypot(map_axis(x, w), map_axis(y, h));
            const auto v = mitchell_(d / scale);
            value = v;
            sum += v;
        }
    }
    normalize(kernel, sum, w, h);
}

void kernel::lanczos(std::complex<double> *kernel, const int w, const int h,
    const double scale, const int a) {
    using namespace std;
    auto sum = 0.0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            auto &value = kernel[y * w + x];
            if (x == 0 && y == 0) {
                value = 1.0;
                sum += 1.0;
                continue;
            }
            if (x > a || y > a) {
                value = 0.0;
                continue;
            }
            const auto sx = map_axis(x, w) * scale;
            const auto sxa = sx / a;
            const auto sy = map_axis(y, h) * scale;
            const auto sya = sy / a;
            const auto Lxy = (sin(sx) / sx * (sin(sxa) / sxa)) *
                             (sin(sy) / sy * (sin(sya) / sya));
            value = Lxy;
            sum += Lxy;
        }
    }
    normalize(kernel, sum, w, h);
}
