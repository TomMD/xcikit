# ------------ #
# Libraries    #
# ------------ #

add_library(xci-widgets
    Button.cpp
    Checkbox.cpp
    FpsDisplay.cpp
    Icon.cpp
    Theme.cpp
    Widget.cpp
    TextInput.cpp
    Form.cpp
    Label.cpp
    TextTerminal.cpp
    )

target_link_libraries(xci-widgets
    xci-text)

if (APPLE)
    set_target_properties(xci-widgets PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup")
endif()

if (BUILD_FRAMEWORKS)
    set_target_properties(xci-widgets PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A
        MACOSX_FRAMEWORK_IDENTIFIER cz.xci.widgets
        # "current version" in semantic format in Mach-O binary file
        VERSION 1.0.0
        # "compatibility version" in semantic format in Mach-O binary file
        SOVERSION 1.0.0
        PUBLIC_HEADER xci-widgets.h
        )
endif()

install(TARGETS xci-widgets EXPORT xcikit DESTINATION lib)
