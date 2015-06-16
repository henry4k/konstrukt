add_custom_command(OUTPUT flextGL.h flextGL.c spec/gl.xml
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/flextGLgen.py
                                    -t ${CMAKE_CURRENT_SOURCE_DIR}/template
                                    -D ${CMAKE_CURRENT_BINARY_DIR}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/profile.cfg
    DEPENDS flextGLgen.py
            profile.cfg)

add_library(flextGL STATIC flextGL.h flextGL.c)
