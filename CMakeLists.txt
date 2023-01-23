cmake_minimum_required(VERSION 3.6)
project(siren_core)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SIREN_TESTS "Build Siren with tests" TRUE)
option(PROFILING "Build Siren with DTrace flags" FALSE)

if (PROFILING)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")
endif ()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

#exclude kissfft tests
option(KISSFFT_TEST "Build kissfft tests" ON)
set(KISSFFT_TEST OFF)
option(KISSFFT_TOOLS "Build kissfft command-line tools" ON)
set(KISSFFT_TOOLS OFF)


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
        )

add_library(miniaudio STATIC vendor/miniaudio/extras/miniaudio_split/miniaudio.c)
set_target_properties(miniaudio PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(siren_core Eigen3::Eigen kissfft miniaudio)
target_include_directories(siren_core PRIVATE vendor/wasserstein)


if (BUILD_SIREN_TESTS)
    add_executable(test_decoder test/decoder.cpp)
    add_executable(test_entities test/entities.cpp)
    add_executable(test_fft test/fft.cpp)

    set(libs gtest gtest_main siren_core)

    target_link_libraries(test_decoder ${libs})
    target_link_libraries(test_entities ${libs})
    target_link_libraries(test_fft ${libs})
endif ()