cmake_minimum_required(VERSION 3.1)

project(konstrukt)


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
                    ${PhysFS_INCLUDE_DIRS})


### Further definitions:

add_subdirectory(third-party)
add_subdirectory(engine)


### Distribution:

string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWERCASE_SYSTEM_NAME)
configure_file(package.json.in
               ${CMAKE_CURRENT_BINARY_DIR}/package.json @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/package.json
        DESTINATION .)

install(DIRECTORY core
        DESTINATION .)

include(CPack)
