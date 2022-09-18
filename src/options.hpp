#ifndef IMAGECONV_OPTIONS_HPP
#define IMAGECONV_OPTIONS_HPP

#include <string>

enum class method_type {
    nop,
    gaussian,
    spectrum,
    downscale2x,
    upscale2x,
};

struct options {
    std::string input, output;
    double weight;
    unsigned extend;
    method_type method;

    options();

    std::string get_method_str() const;
    bool set_method_str(const std::string &);

    void check();
};

#endif // IMAGECONV_OPTIONS_HPP
