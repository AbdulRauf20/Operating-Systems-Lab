cmake_minimum_required(VERSION 3.10)

project(StaticDemo)

add_library(mathlib STATIC mathlib.c)

add_executable(app main.c)

target_link_libraries(app mathlib)
