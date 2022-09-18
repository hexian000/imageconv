#include "options.hpp"
#include "option_error.hpp"

#include <boost/format.hpp>

#include <cmath>
#include <cstring>
#include <vector>

options::options() : weight(10.0), extend(64), method(method_type::gaussian) {}

static inline bool is_sane(double x) {
    return !(std::isnan(x) || std::isinf(x));
}

void options::check() {
#define THROW_INVALID(x)                                                       \
    throw option_error(                                                        \
        (boost::format("invalid value for '" #x "' - %d") % (x)).str())

    if (!is_sane(weight) || (weight <= 0.0)) { THROW_INVALID(weight); }

#undef THROW_INVALID
}

namespace {
const std::vector<std::pair<method_type, std::string>> method_strings = {
    {method_type::nop, "nop"},
    {method_type::gaussian, "gaussian"},
    {method_type::spectrum, "spectrum"},
    {method_type::downscale2x, "downscale2x"},
    {method_type::upscale2x, "upscale2x"},
};
}

bool options::set_method_str(const std::string &s) {
    for (const auto &it : method_strings) {
        if (s == it.second) {
            method = it.first;
            switch (method) {
            case method_type::spectrum: extend = 0; break;
            default: break;
            }
            return true;
        }
    }
    return false;
}

std::string options::get_method_str() const {
    for (const auto &it : method_strings) {
        if (method == it.first) { return it.second; }
    }
    return "<unknown>";
}
