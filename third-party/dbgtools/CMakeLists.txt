add_library(dbgtools STATIC callstack.cpp)
if(WIN32)
    target_link_libraries(dbgtools dbghelp)
endif()
