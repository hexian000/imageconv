#ifndef IMAGECONV_METHODS_HPP
#define IMAGECONV_METHODS_HPP

#include <complex>

namespace methods {
void copy(std::complex<double> *dst, std::complex<double> *src, int n);

void multiply(
    std::complex<double> *a, const std::complex<double> *k, const int n);

void lowpass(std::complex<double> *a, int w, int h, int w1, int h1);

void fftshift(
    std::complex<double> *dst, std::complex<double> *src, int w, int h);

void ifftshift(
    std::complex<double> *dst, std::complex<double> *src, int w, int h);

void spectrum(std::complex<double> *dst_r, std::complex<double> *dst_g,
    std::complex<double> *dst_b, const std::complex<double> *src_r,
    const std::complex<double> *src_g, const std::complex<double> *src_b,
    int width, int height);

void downsample2x(
    std::complex<double> *dst, std::complex<double> *src, int w0, int h0);

void upsample2x(
    std::complex<double> *dst, std::complex<double> *src, int w0, int h0);
} // namespace methods

#endif // IMAGECONV_METHODS_HPP
