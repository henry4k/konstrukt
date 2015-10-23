option(APOAPSIS_GL_DEBUG_FIX "Enable if compiler complains about missing OnDebugEvent function.")
if(APOAPSIS_GL_DEBUG_FIX)
    add_definitions(-DAPOAPSIS_GL_DEBUG_FIX)
endif()

option(APOAPSIS_DEBUG_MESH "Generate and draw debug vectors for mesh vertices.")
if(APOAPSIS_DEBUG_MESH)
    add_definitions(-DAPOAPSIS_DEBUG_MESH)
endif()

file(GLOB Sources *.h
                  *.cpp
                  control_bindings/*.h
                  control_bindings/*.cpp
                  lua_bindings/*.h
                  lua_bindings/*.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Constants.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/Constants.h @ONLY)

add_executable(apoapsis ${Sources})

target_compile_features(apoapsis PRIVATE cxx_lambdas)

install(TARGETS apoapsis
        RUNTIME DESTINATION bin)

include_directories("${CMAKE_CURRENT_BINARY_DIR}"
                    "${CMAKE_BINARY_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party/inih"
                    "${PROJECT_SOURCE_DIR}/third-party/lua-cjson"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_image")

target_link_libraries(apoapsis stbimage
                               inih
                               lua-cjson
                               flextGL
                               ${OPENGL_gl_LIBRARY}
                               ${LUA_LIBRARY}
                               ${GLFW3_LIBRARIES}
                               ${OPENAL_LIBRARY}
                               ${ALURE_LIBRARIES}
                               ${BULLET_COLLISION_LIBRARY}
                               ${BULLET_DYNAMICS_LIBRARY}
                               ${BULLET_MATH_LIBRARY}
                               ${PhysFS_LIBRARIES})

add_subdirectory(tests)
