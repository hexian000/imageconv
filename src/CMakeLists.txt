add_executable(imageconv
    main.cpp
    options.cpp options.hpp option_error.hpp
    imageconv.cpp imageconv.hpp
    fft.cpp fft.hpp
    methods.cpp methods.hpp
    kernel.cpp kernel.hpp
    image.cpp image.hpp)

target_link_libraries(imageconv boost_program_options fftw3 png z m pthread)
