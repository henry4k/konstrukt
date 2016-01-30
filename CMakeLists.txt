cmake_minimum_required(VERSION 2.8)

project(konstrukt)

### Options:

set(KONSTRUKT_SCRIPT_INSTALL_PATH "share/konstrukt"
    CACHE PATH "Where the scripts shall be installed.")


### Dependencies:

list(INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(GLFW3 3.1 REQUIRED)
find_package(PhysFS REQUIRED)
find_package(Bullet 2.80 REQUIRED)
find_package(OpenAL 1 REQUIRED)
find_package(ALURE 1.2 REQUIRED)
if(CMAKE_VERSION VERSION_LESS 3)
    set(LUA_INCLUDE_DIR NOTFOUND CACHE PATH "")
    set(LUA_LIBRARY NOTFOUND CACHE PATH "")
    if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARY)
        message(SEND_ERROR "LUA_INCLUDE_DIR or LUA_LIBRARY missing")
    endif()
else()
    find_package(Lua 5.2 REQUIRED)
endif()

include_directories(${OPENGL_INCLUDE_DIR}
                    ${LUA_INCLUDE_DIR}
                    ${GLFW3_INCLUDE_DIRS}
                    ${OPENAL_INCLUDE_DIR}
                    ${ALURE_INCLUDE_DIRS}
                    ${BULLET_INCLUDE_DIR}
                    ${PHYSFS_INCLUDE_DIRS})


### Further definitions:

add_subdirectory(third-party)
add_subdirectory(engine)

install(DIRECTORY core
        DESTINATION "${KONSTRUKT_SCRIPT_INSTALL_PATH}")

include(CPack)
