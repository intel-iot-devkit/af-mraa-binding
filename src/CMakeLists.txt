add_library (mraa-binding
             MODULE
             af-mraa-binding.c
)

# Binder exposes a unique public entry point
set_target_properties (mraa-binding PROPERTIES
        PREFIX "af-"
        LABELS "BINDING"
        OUTPUT_NAME mraa-binding
)

target_include_directories (mraa-binding
                            PUBLIC
                            ${MRAA_INCLUDE_DIR}
                            ${JSON-C_INCLUDE_DIR}
)

target_link_libraries (mraa-binding
                       ${MRAA_LIBRARIES}
                       ${JSON-C_LIBRARIES}
                       jstruct
)
