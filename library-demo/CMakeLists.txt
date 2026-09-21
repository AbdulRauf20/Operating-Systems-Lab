cmake_minimum_required(VERSION 3.10)

project(SharedDemo)

add_library(mathlib SHARED mathlib.c)

add_executable(app main.c)

target_link_libraries(app mathlib)
