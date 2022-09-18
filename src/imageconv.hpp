#ifndef IMAGECONV_IMAGECONV_HPP
#define IMAGECONV_IMAGECONV_HPP

#include "options.hpp"

class imageconv {
    options option;

public:
    explicit imageconv(const options &);

    void run() const;
};

#endif // IMAGECONV_IMAGECONV_HPP
