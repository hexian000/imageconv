# imageconv

imageconv correctly computes full size image convolution for experimental purpose.

## Key Points

- Convolve with full-size kernel. (powered by FFT)
- Gamma corrected
- 2D spectrum visualization
- Written in modern C++

See also [convolution theorem](https://en.wikipedia.org/wiki/Convolution_theorem).

## Dependencies

```sh
# Debian/Ubuntu
apt install libboost-all-dev libpng-dev libfftw3-dev
# Alpine Linux
apk add boost-dev libpng-dev fftw-dev
```

## Building

It's very straightforward to build it with cmake.

```sh
git clone https://github.com/hexian000/imageconv.git
mkdir "build"
cmake -DCMAKE_BUILD_TYPE="Release" \
    -S "imageconv" -B "build"
cmake --build "build" --parallel
```

See also [m.sh](m.sh);

## Usage

```sh
# Let's see the perfect blur
./imageconv -m gaussian -i 0.png -o 0.gaussian.png

# Check the spectrum
./imageconv -m spectrum -i 0.png -o 0.spectrum.png

# Downscale image in spectral domain
./imageconv -m downscale2x -i 0.png -o 1.png
./imageconv -m downscale2x -i 1.png -o 2.png
./imageconv -m downscale2x -i 2.png -o 3.png
```
