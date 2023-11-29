cmake_minimum_required(VERSION 3.6.0)
project(ifma_app_binderceptor_native_jni)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# self include
include_directories("inc")
# ifma include
include_directories(../../../../../header/include/)

file(GLOB foo_sources . *.cpp *.h
        inc/*.h
        src/*.cpp
)

set(SOURCE_FILES ${foo_sources})
add_executable(ifma_app_binderceptor_native_jni ${SOURCE_FILES})
