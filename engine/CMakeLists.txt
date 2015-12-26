file(GLOB Sources *.h
                  *.cpp
                  control_bindings/*.h
                  control_bindings/*.cpp
                  lua_bindings/*.h
                  lua_bindings/*.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Constants.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/Constants.h @ONLY)

add_executable(apoapsis ${Sources})

install(TARGETS apoapsis
        RUNTIME DESTINATION bin)

include_directories("${CMAKE_CURRENT_BINARY_DIR}"
                    "${CMAKE_BINARY_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party/inih"
                    "${PROJECT_SOURCE_DIR}/third-party/lua-cjson"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_image_resize"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_image")

target_link_libraries(apoapsis stbimage
                               stbimageresize
                               inih
                               lua-cjson
                               flextGL
                               ${LUA_LIBRARY}
                               ${OPENAL_LIBRARY}
                               ${ALURE_LIBRARIES}
                               ${BULLET_DYNAMICS_LIBRARY}
                               ${BULLET_COLLISION_LIBRARY}
                               ${BULLET_MATH_LIBRARY}
                               ${PhysFS_LIBRARIES}
                               ${GLFW3_LIBRARIES}
                               ${OPENGL_gl_LIBRARY})

option(APOAPSIS_SELFCONTAINED "Everything is located besides the binary.")
if(APOAPSIS_SELFCONTAINED)
    target_compile_definitions(apoapsis PRIVATE APOAPSIS_SELFCONTAINED)
endif()

option(APOAPSIS_GL_DEBUG_FIX "Enable if compiler complains about missing OnDebugEvent function.")
if(APOAPSIS_GL_DEBUG_FIX)
    target_compile_definitions(apoapsis PRIVATE APOAPSIS_GL_DEBUG_FIX)
endif()

option(APOAPSIS_DEBUG_MESH "Generate and draw debug vectors for mesh vertices.")
if(APOAPSIS_DEBUG_MESH)
    target_compile_definitions(apoapsis PRIVATE APOAPSIS_DEBUG_MESH)
endif()

option(APOAPSIS_STRICT_COMPILER "Enables compiler warnings and has them treated as errors.  Recommended for developers.")
if(APOAPSIS_STRICT_COMPILER)
    if(MSVC)
        target_compile_options(apoapsis PRIVATE /W4 /WX)
    else()
        target_compile_options(apoapsis PRIVATE -Wall -Wpedantic -Werror)
    endif()
endif()

add_subdirectory(tests)
