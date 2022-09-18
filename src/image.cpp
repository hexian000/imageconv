#include "image.hpp"

#include <array>
#include <cmath>
#include <complex>
#include <iostream>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>

struct image_private {
    boost::gil::rgb8_image_t image;
    const double gamma;

    explicit image_private(const std::string &path)
        : gamma(boost::gil::read_image_info(path, boost::gil::png_tag{})
                    ._info._file_gamma) {
        boost::gil::read_image(path, image, boost::gil::png_tag{});
    }

    explicit image_private(
        const std::size_t width, const std::size_t height, const double gamma)
        : image(width, height), gamma(gamma) {}
};

image::image(const std::string &filename)
    : p(std::make_unique<image_private>(filename)),
      gamma(p->gamma > 0.0 ? p->gamma : default_gamma) {}

image::image(const std::size_t width, const std::size_t height)
    : p(std::make_unique<image_private>(width, height, default_gamma)),
      gamma(default_gamma) {}

image::~image() = default;

std::tuple<std::size_t, std::size_t> image::get_size() const {
    const auto &image = p->image;
    return {image.width(), image.height()};
}

std::tuple<std::size_t, std::size_t> image::get_extended_size(
    const unsigned px) const {
    const auto &image = p->image;
    return {image.width() + px * 2, image.height() + px * 2};
}

namespace {
class color_decoder final {
    double table[256];

public:
    explicit color_decoder(const double decode_gamma) {
        for (int i = 0; i < 256; i++) {
            table[i] = std::pow(i / 255.0, decode_gamma);
        }
    }

    inline double operator()(const unsigned char b) const { return table[b]; }
};

class color_encoder final {
    const double gamma;

public:
    explicit color_encoder(const double encode_gamma) : gamma(encode_gamma) {}

    inline unsigned char operator()(const std::complex<double> &c) const {
        double v = std::pow(c.real(), gamma);
        if (v <= 0.0) { return 0u; }
        v *= 255.0;
        if (v >= 255.0) { return 255u; }
        return static_cast<unsigned char>(v);
    }
};
} // namespace

void image::load(std::complex<double> *r, std::complex<double> *g,
    std::complex<double> *b) const {
    const auto &image = p->image;
    const std::size_t width = image.width();
    const std::size_t height = image.height();
    const color_decoder decode{1.0 / default_gamma};

    const auto view = boost::gil::const_view(image);
    for (std::size_t y = 0; y < height; y++) {
        const auto it = view.row_begin(y);
        for (std::size_t x = 0; x < width; x++) {
            const auto &pixel = it[x];
            r[y * width + x] = decode(boost::gil::at_c<0>(pixel));
            g[y * width + x] = decode(boost::gil::at_c<1>(pixel));
            b[y * width + x] = decode(boost::gil::at_c<2>(pixel));
        }
    }
}

void image::load_extended(const unsigned extend, std::complex<double> *r,
    std::complex<double> *g, std::complex<double> *b) const {
    const auto &image = p->image;
    const std::size_t width = image.width();
    const std::size_t height = image.height();
    const color_decoder decode{1.0 / default_gamma};
    const auto max_x = width + extend * 2;
    const auto max_y = height + extend * 2;

    const auto clamp_i = [extend](const std::size_t i, const std::size_t mx) {
        if (i < extend) { return extend - i; }
        if (i < extend + mx) { return i - extend; }
        return mx * 2 + extend - i - 1;
    };

    const auto view = boost::gil::const_view(image);
    for (std::size_t y = 0; y < max_y; y++) {
        const auto iy = clamp_i(y, height);
        const auto it = view.row_begin(iy);
        for (std::size_t x = 0; x < max_x; x++) {
            const auto ix = clamp_i(x, width);
            const auto &pixel = it[ix];
            const auto i = y * max_x + x;
            r[i] = decode(boost::gil::at_c<0>(pixel));
            g[i] = decode(boost::gil::at_c<1>(pixel));
            b[i] = decode(boost::gil::at_c<2>(pixel));
        }
    }
}

void image::save(const std::complex<double> *r, const std::complex<double> *g,
    const std::complex<double> *b) {
    auto &image = p->image;
    const std::size_t width = image.width();
    const std::size_t height = image.height();
    const color_encoder encode{gamma};

    const auto view = boost::gil::view(image);
    for (std::size_t y = 0; y < height; y++) {
        const auto it = view.row_begin(y);
        for (std::size_t x = 0; x < width; x++) {
            auto &pixel = it[x];
            const auto i = y * width + x;
            boost::gil::at_c<0>(pixel) = encode(r[i]);
            boost::gil::at_c<1>(pixel) = encode(g[i]);
            boost::gil::at_c<2>(pixel) = encode(b[i]);
        }
    }
}

void image::save_extended(const unsigned extend, const std::complex<double> *r,
    const std::complex<double> *g, const std::complex<double> *b) {
    auto &image = p->image;
    const std::size_t width = image.width();
    const std::size_t height = image.height();
    const color_encoder encode{gamma};
    const auto max_x = width + extend * 2;

    const auto view = boost::gil::view(image);
    for (std::size_t y = 0; y < height; y++) {
        const auto it = view.row_begin(y);
        for (std::size_t x = 0; x < width; x++) {
            auto &pixel = it[x];
            const auto ix = x + extend, iy = y + extend;
            const auto i = iy * max_x + ix;
            boost::gil::at_c<0>(pixel) = encode(r[i]);
            boost::gil::at_c<1>(pixel) = encode(g[i]);
            boost::gil::at_c<2>(pixel) = encode(b[i]);
        }
    }
}

void image::write(const std::string &filename) const {
    boost::gil::write_view(
        filename, boost::gil::const_view(p->image), boost::gil::png_tag{});
}
