cmake_minimum_required(VERSION 3.6)
project(siren_core)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SIREN_TESTS "Build Siren with tests" TRUE)
option(PROFILING "Build Siren with DTrace flags" FALSE)

#exclude kissfft tests
option(KISSFFT_TEST "Build kissfft tests" OFF)
option(KISSFFT_TOOLS "Build kissfft command-line tools" OFF)

add_subdirectory(vendor/eigen)
add_subdirectory(vendor/googletest)
add_subdirectory(vendor/kissfft)

add_library(siren_core STATIC
        src/entities/freq_bin.cpp
        src/entities/freq_bin.h
        src/decoder/pcm.cpp
        src/decoder/pcm.h
        src/fft/fft.cpp
        src/fft/fft.h
        src/entities/spectrogram.cpp
        src/entities/spectrogram.h
        src/entities/fingerprint.h
        src/serializer/traits.h
        src/serializer/serializer.h
        src/common/common.cpp
        src/common/common.h
        src/siren.cpp
        src/siren.h
        src/client_wrapper/client_wrapper.cpp
        src/client_wrapper/client_wrapper.h
        )

add_library(miniaudio STATIC vendor/miniaudio/extras/miniaudio_split/miniaudio.c)
set_target_properties(miniaudio PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(siren_core Eigen3::Eigen kissfft miniaudio)

if (BUILD_SIREN_TESTS)
set(TEST_SRC test/decoder.cpp test/entities.cpp test/core.cpp test/assert.cpp)
set(test_libs gtest gtest_main siren_core)
set(i 0)

function(add_test_file TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE ${test_libs})
endfunction()

foreach(file ${TEST_SRC})
    add_test_file("siren_test_${i}" ${file})
math(EXPR i "${i} + 1")
endforeach()

endif()