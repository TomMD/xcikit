find_package(replxx REQUIRED)

# Fire Script compiler/interpreter
add_executable(fire
    BytecodeTracer.cpp
    Context.cpp
    ReplCommand.cpp
    main.cpp
    )
target_link_libraries(fire
    xci-core
    xci-script
    replxx::replxx
    )
install(TARGETS fire EXPORT xcikit DESTINATION bin)
