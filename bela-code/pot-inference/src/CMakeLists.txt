set(EXE_NAME pot-inference)

file(GLOB SRC_FILES *.cpp)

add_executable(${EXE_NAME} ${SRC_FILES})

target_include_directories(
    ${EXE_NAME} PRIVATE ${BELA_ROOT} ${BELA_ROOT}/include
)

file(GLOB_RECURSE LIBRARY_SRC_FILES "${BELA_ROOT}/libraries/**/*.cpp")
target_sources(${EXE_NAME} PRIVATE ${LIBRARY_SRC_FILES})
# Include all subdirectories in ${BELA_ROOT}/libraries
file(GLOB LIBRARY_INCLUDE_DIRS "${BELA_ROOT}/libraries/*")
foreach(LIB_DIR ${LIBRARY_INCLUDE_DIRS})
    if(IS_DIRECTORY ${LIB_DIR})
        target_include_directories(${EXE_NAME} PRIVATE ${LIB_DIR})
    endif()
endforeach()

target_link_libraries(
    ${EXE_NAME}
    PRIVATE
    ${BELA_ROOT}/lib/libbelafull.so
    ${COBALT_LIB}
    ${NEON_LIB}
    ${MATHNEON_LIB}
    dl
    prussdrv
    asound
    atomic
    sndfile
    pthread
)

if (ENABLE_PYTORCH_FRONTEND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
    target_link_libraries(${EXE_NAME} PRIVATE ${TORCH_LIBRARIES})
endif()