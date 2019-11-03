project(memanalyze)

add_executable(jmemanalyze memanalyze.c)

target_link_libraries(jmemanalyze PUBLIC jcommon)

install(TARGETS jmemanalyze RUNTIME DESTINATION bin)