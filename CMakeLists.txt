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

#exclude kissfft tests
option(KISSFFT_TEST "Build kissfft tests" OFF)
option(KISSFFT_TOOLS "Build kissfft command-line tools" OFF)


add_subdirectory(vendor/eigen)
add_subdirectory(vendor/googletest)
add_subdirectory(vendor/kissfft)
add_subdirectory(vendor/restclient-cpp)

find_package(OpenCV REQUIRED)

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
        src/common/common.cpp
        src/common/common.h
        src/serializer/serializer.h
        src/serializer/traits.h
        src/siren.cpp
        src/siren.h
        )

add_library(miniaudio STATIC vendor/miniaudio/extras/miniaudio_split/miniaudio.c)
set_target_properties(miniaudio PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(siren_core ${OpenCV_LIBS} Eigen3::Eigen kissfft miniaudio)

add_library(client_wrapper STATIC

        src/client_wrapper/abstract_request.h
        src/client_wrapper/abstract_http_client.h
        src/client_wrapper/abstract_http_client.cpp

        src/client_wrapper/http_client_factory.h
        src/client_wrapper/http_client_factory.cpp

        src/client_wrapper/rest/rest_client.h
        src/client_wrapper/rest/rest_client.cpp
        src/client_wrapper/rest/rest_request.h
        src/client_wrapper/rest/rest_request.cpp

        src/client_wrapper/grpc/grpc_client.cpp
        src/client_wrapper/grpc/grpc_client.h
        src/client_wrapper/grpc/grpc_request.cpp
        src/client_wrapper/grpc/grpc_request.h

        src/client_wrapper/client_wrapper.cpp
        src/client_wrapper/client_wrapper.h
        )

add_subdirectory(src/proto)

target_link_libraries(client_wrapper siren_core_proto siren_core restclient-cpp)
target_include_directories(client_wrapper PUBLIC ${CMAKE_BINARY_DIR})

if (BUILD_SIREN_TESTS)
    add_executable(test_decoder test/decoder.cpp)
    add_executable(test_entities test/entities.cpp)

    set(libs gtest gtest_main client_wrapper)

    target_link_libraries(test_decoder ${libs})
    target_link_libraries(test_entities ${libs})
endif ()