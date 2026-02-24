idf_component_register(
    SRCS
        "berry_espidf_project.c"
        "berry_runtime.c"
        "berry_modules/mod_system.c"

    INCLUDE_DIRS "." "berry_modules"

    REQUIRES
        berry
        esp_partition
        esp_system
        heap
)