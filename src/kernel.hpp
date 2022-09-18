#ifndef IMAGECONV_KERNEL_HPP
#define IMAGECONV_KERNEL_HPP

#include <complex>

class kernel final {
public:
    static void identity(std::complex<double> *kernel, int width, int height);

    static void gaussian(
        std::complex<double> *kernel, int width, int height, double weight);

    static void lanczos(
        std::complex<double> *kernel, int w, int h, double scale, int a);

    static void mitchell(
        std::complex<double> *kernel, int w, int h, double scale);
};

#endif // IMAGECONV_KERNEL_HPP
