file(GLOB_RECURSE BERRY_SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/generate/*.c
)

idf_component_register(
    SRCS ${BERRY_SRC}
    be_compat.c
    INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/generate
        ${CMAKE_CURRENT_LIST_DIR}/default
    
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error
    -Wno-char-subscripts
)