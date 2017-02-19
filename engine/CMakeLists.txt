file(GLOB LibSources
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.cpp
     control_bindings/*.cpp
     lua_bindings/*.cpp
     vfs/*.cpp)
list(REMOVE_ITEM LibSources ExecutableMain.cpp)

add_library(konstruktlib STATIC ${LibSources})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Constants.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/Constants.h @ONLY)

include_directories("${CMAKE_CURRENT_BINARY_DIR}"
                    "${CMAKE_BINARY_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party"
                    "${PROJECT_SOURCE_DIR}/third-party/inih"
                    "${PROJECT_SOURCE_DIR}/third-party/lua-cjson"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_image_resize"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_image"
                    "${PROJECT_SOURCE_DIR}/third-party/stb_sprintf"
                    "${PROJECT_SOURCE_DIR}/third-party/tinycthread")

target_link_libraries(konstruktlib stb_image
                                   stb_image_resize
                                   stb_sprintf
                                   inih
                                   lua-cjson
                                   flextGL
                                   tinycthread
                                   ${LUA_LIBRARY}
                                   ${OPENAL_LIBRARY}
                                   ${ALURE_LIBRARIES}
                                   ${BULLET_DYNAMICS_LIBRARY}
                                   ${BULLET_COLLISION_LIBRARY}
                                   ${BULLET_MATH_LIBRARY}
                                   ${PhysFS_LIBRARIES}
                                   ${GLFW3_LIBRARIES}
                                   ${OPENGL_gl_LIBRARY})

add_executable(konstrukt ExecutableMain.cpp)
target_link_libraries(konstrukt konstruktlib)

install(TARGETS konstrukt
        RUNTIME DESTINATION bin)

option(KONSTRUKT_SELFCONTAINED "Everything is located besides the binary.")
if(KONSTRUKT_SELFCONTAINED)
    target_compile_definitions(konstruktlib PRIVATE KONSTRUKT_SELFCONTAINED)
endif()

option(KONSTRUKT_GL_DEBUG_FIX "Enable if compiler complains about missing OnDebugEvent function.")
if(KONSTRUKT_GL_DEBUG_FIX)
    target_compile_definitions(konstruktlib PRIVATE KONSTRUKT_GL_DEBUG_FIX)
endif()

option(KONSTRUKT_DEBUG_MESH "Generate and draw debug vectors for mesh vertices.")
if(KONSTRUKT_DEBUG_MESH)
    target_compile_definitions(konstruktlib PRIVATE KONSTRUKT_DEBUG_MESH)
endif()

option(KONSTRUKT_STRICT_COMPILER "Enables compiler warnings and has them treated as errors.  Recommended for developers.")
if(KONSTRUKT_STRICT_COMPILER)
    if(MSVC)
        target_compile_options(konstruktlib PRIVATE /W4 /WX)
    else()
        target_compile_options(konstruktlib PRIVATE -std=c++11 -Wall -Wpedantic -Werror)
    endif()
endif()

add_subdirectory(tests)
