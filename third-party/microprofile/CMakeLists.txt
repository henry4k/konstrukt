find_package(Threads REQUIRED)

add_library(microprofile STATIC microprofile.cpp)
include_directories("${CMAKE_BINARY_DIR}/third-party") # needs access to flextGL
target_link_libraries(microprofile flextGL
                                   ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
    target_link_libraries(microprofile wsock32 ws2_32)
endif()
