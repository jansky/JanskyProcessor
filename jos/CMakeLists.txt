project(jos)

add_custom_target(jos ALL
        COMMAND sh -c "JLINK=\"${CMAKE_BINARY_DIR}${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/linker/jlink '.text;.dta'\" JSASM=${CMAKE_BINARY_DIR}${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sasm/jsasm make -f JOSMakefile"
        SOURCES dta.s exit.s hello.s jos.s pow.s shell.s strlen.s strlen-new.s syscall.s
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/jos
        VERBATIM
        )

set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES *.o *.s *.sp.lbls *.bin)

