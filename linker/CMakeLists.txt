project(linker)

add_executable(jlink io.c labels.c main.c section.c)

target_link_libraries(jlink PUBLIC jcommon)

install(TARGETS jlink RUNTIME DESTINATION bin)