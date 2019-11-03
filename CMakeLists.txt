cmake_minimum_required(VERSION 3.10)

project(JanskyProcessor)

include_directories(include)

add_subdirectory(common)
add_subdirectory(emulator)
add_subdirectory(disassembler)
add_subdirectory(sasm)
add_subdirectory(linker)
add_subdirectory(memanalyze)
add_subdirectory(jos)

