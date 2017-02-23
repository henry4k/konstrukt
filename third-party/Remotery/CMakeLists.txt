find_package(Threads REQUIRED)

add_library(Remotery STATIC Remotery.c)
target_link_libraries(Remotery ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
    target_link_libraries(Remotery wsock32 ws2_32)
endif()

if(UNIX AND NOT CYGWIN)
    target_link_libraries(Remotery dl)
endif ( )
