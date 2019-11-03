project(disassembler)

add_executable(jdisassembler disassembler.c)

target_link_libraries(jdisassembler PUBLIC jcommon)

install(TARGETS jdisassembler RUNTIME DESTINATION bin)