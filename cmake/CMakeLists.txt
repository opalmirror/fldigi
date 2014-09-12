project(TestFLTK)
cmake_minimum_required(VERSION 2.8)
# Add local cmake modules to the search path.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(FLTK_DEBUG TRUE)
#find_package(FLTK)
include(FindFLTK.cmake)
