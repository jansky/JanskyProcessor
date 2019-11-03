project(emulator)

add_executable(jemulator main.c debug.c)

target_link_libraries(jemulator PUBLIC jcommon)

install(TARGETS jemulator RUNTIME DESTINATION bin)