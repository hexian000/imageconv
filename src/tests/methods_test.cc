#include <complex>
#include <iostream>
#include <random>

#include "../methods.hpp"

int main() {
    using namespace std;
    random_device rd;
    mt19937_64 mt(rd());
    uniform_real_distribution<double> dist(-1.0, 1.0);
    auto fftshift_fuzz = [&](int w, int h) {
        auto *n = new double[w * h];
        auto *a = new complex<double>[w * h];
        auto *t = new complex<double>[w * h];
        for (int i = 0; i < w * h; i++) {
            a[i] = n[i] = dist(mt);
        }
        methods::fftshift(a, t, w, h);
        methods::ifftshift(a, t, w, h);
        for (int i = 0; i < w * h; i++) {
            if (a[i].real() != n[i] || a[i].imag() != 0) {
                printf("fail on w=%d h=%d\n", w, h);
                exit(EXIT_FAILURE);
            }
        }
        delete[] a, delete[] t;
    };
    fftshift_fuzz(16, 16);
    fftshift_fuzz(15, 15);
    fftshift_fuzz(16, 15);
    fftshift_fuzz(15, 16);
    return 0;
}
