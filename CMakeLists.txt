cmake_minimum_required(VERSION 3.1)

project(konstrukt)

### Configuration variables:

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
find_package(Lua 5.2 EXACT REQUIRED)

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


### Distribution:

string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWERCASE_SYSTEM_NAME)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json.in
               ${CMAKE_CURRENT_BINARY_DIR}/package.json @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/meta.json
        DESTINATION "${KONSTRUKT_SCRIPT_INSTALL_PATH}")

install(DIRECTORY core
        DESTINATION "${KONSTRUKT_SCRIPT_INSTALL_PATH}")

include(CPack)
