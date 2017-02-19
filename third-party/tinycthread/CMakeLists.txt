find_package(Threads REQUIRED)

add_library(tinycthread STATIC tinycthread.c)
target_link_libraries(tinycthread ${CMAKE_THREAD_LIBS_INIT})
