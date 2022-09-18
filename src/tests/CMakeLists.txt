enable_testing()

add_executable(methods_test methods_test.cc ../methods.hpp ../methods.cpp)
target_link_libraries(methods_test m)

add_test(NAME methods COMMAND ${CMAKE_CURRENT_BINARY_DIR}/methods_test)

add_executable(kernels_test kernels_test.cc ../kernel.hpp ../kernel.cpp)
target_link_libraries(kernels_test m)

add_test(NAME kernels COMMAND ${CMAKE_CURRENT_BINARY_DIR}/kernels_test)
