cmake_minimum_required(VERSION 2.8)

project(apoapsis)

### Options:

option(BUILD_TESTS "Build native tests" OFF)
set(SCRIPT_INSTALL_PATH "share/apoapsis"
    CACHE PATH "Where the scripts shall be installed.") # TODO


### Dependencies:

list(INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(GLFW3 3.1 REQUIRED)
find_package(PhysFS 2.1 REQUIRED)
find_package(Lua 5.2 REQUIRED)
find_package(Bullet 2.80 REQUIRED)
find_package(OpenAL 1 REQUIRED)
find_package(ALURE 1.2 REQUIRED)
find_package(GLM 0.9 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR}
                    ${LUA_INCLUDE_DIR}
                    ${GLFW3_INCLUDE_DIRS}
                    ${OPENAL_INCLUDE_DIR}
                    ${ALURE_INCLUDE_DIRS}
                    ${BULLET_INCLUDE_DIR}
                    ${PHYSFS_INCLUDE_DIRS}
                    ${GLM_INCLUDE_DIRS}) # Because GLM consists solely of headers


### Further definitions:

add_subdirectory(third-party)
add_subdirectory(engine)

install(DIRECTORY core
        DESTINATION "${SCRIPT_INSTALL_PATH}")
