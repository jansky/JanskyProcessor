project(sasm)

add_executable(jsasm asm.c io.c labels.c location.c main.c section.c)

target_link_libraries(jsasm PUBLIC jcommon)

install(TARGETS jsasm RUNTIME DESTINATION bin)