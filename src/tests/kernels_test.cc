#include <complex>
#include <iostream>
#include <random>

#include "../kernel.hpp"

int main() {
    using namespace std;

    const int w = 16, h = 16;
    auto k = new std::complex<double>[w * h];
    kernel::lanczos(k, w, h, 2.0, 10);

    return 0;
}
