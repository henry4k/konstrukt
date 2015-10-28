find_package(PythonInterp 3.0 REQUIRED)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/flextGL.h"
                          "${CMAKE_CURRENT_BINARY_DIR}/flextGL.c"
                  COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/flextGLgen.py"
                            -t "${CMAKE_CURRENT_SOURCE_DIR}/template"
                            -D "${CMAKE_CURRENT_BINARY_DIR}"
                            "${CMAKE_CURRENT_SOURCE_DIR}/profile.cfg"
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/flextGLgen.py"
                          "${CMAKE_CURRENT_SOURCE_DIR}/profile.cfg")

add_library(flextGL STATIC flextGL.h flextGL.c)
