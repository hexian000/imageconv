#ifndef IMAGECONV_FFT_HPP
#define IMAGECONV_FFT_HPP

#include <complex>
#include <memory>
#include <stack>
#include <vector>

struct fft_private;

class fft final {
    std::unique_ptr<struct fft_private> p;

public:
    fft(std::size_t width, std::size_t height, bool backward = false);

    ~fft();

    void compute(std::shared_ptr<std::complex<double>>);

    static std::shared_ptr<std::complex<double>> new_buffer(std::size_t);
    static void cleanup();
};

#endif // IMAGECONV_FFT_HPP
