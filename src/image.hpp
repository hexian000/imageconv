#ifndef IMAGECONV_IMAGE_HPP
#define IMAGECONV_IMAGE_HPP

#include <complex>
#include <memory>
#include <string>
#include <tuple>

struct image_private;

class image final {
    std::unique_ptr<struct image_private> p;
    static constexpr double default_gamma = 2.2;
    const double gamma;

public:
    explicit image(const std::string &filename);

    image(std::size_t width, std::size_t height);

    ~image();

    [[nodiscard]] std::tuple<std::size_t, std::size_t> get_size() const;

    [[nodiscard]] std::tuple<std::size_t, std::size_t> get_extended_size(
        unsigned extend) const;

    void load(std::complex<double> *r, std::complex<double> *g,
        std::complex<double> *b) const;

    void load_extended(unsigned extend, std::complex<double> *r,
        std::complex<double> *g, std::complex<double> *b) const;

    void save(const std::complex<double> *r, const std::complex<double> *g,
        const std::complex<double> *b);

    void save_extended(unsigned extend, const std::complex<double> *r,
        const std::complex<double> *g, const std::complex<double> *b);

    void write(const std::string &filename) const;
};

#endif // IMAGECONV_IMAGE_HPP
