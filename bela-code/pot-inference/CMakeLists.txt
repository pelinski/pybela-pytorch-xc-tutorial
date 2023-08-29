cmake_minimum_required(VERSION 3.18)

project(pot-inference)

####################################
add_compile_options(
    -march=armv7-a
    -mtune=cortex-a8
    -mfloat-abi=hard
    -mfpu=neon
    -Wno-psabi
)

add_compile_options(
    -O3
    -g 
    -fPIC
    -ftree-vectorize
    -ffast-math
)

add_compile_definitions(DXENOMAI_SKIN_posix)

####################################

set(BELA_ROOT "${CMAKE_SYSROOT}/root/Bela") 
set(SYS_ROOT "${CMAKE_SYSROOT}")

find_library(COBALT_LIB REQUIRED
    NAMES cobalt libcobalt
    HINTS "${CMAKE_SYSROOT}/usr/xenomai/lib"
)

find_library(NEON_LIB REQUIRED
    NAMES NE10 libNE10
    HINTS "${CMAKE_SYSROOT}/usr/lib"
)

find_library(MATHNEON_LIB REQUIRED
    NAMES mathneon libmathneon
    HINTS "${CMAKE_SYSROOT}/usr/lib"
)

####################################

set(ENABLE_PYTORCH_FRONTEND OFF CACHE BOOL "ENABLE_PYTORCH_FRONTEND")

if(ENABLE_PYTORCH_FRONTEND)
    message(STATUS "Enabling PyTorch frontend")
    add_compile_definitions(ENABLE_PYTORCH_FRONTEND)
    # find pytorch
    # -DCMAKE_PREFIX_PATH=/absolute/path/to/libtorch
    list(APPEND CMAKE_PREFIX_PATH /opt/pytorch-install)
    find_package(Torch REQUIRED)
endif()


add_subdirectory(src)
