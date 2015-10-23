option(APOAPSIS_BUILD_TESTS "Build native tests")
if(APOAPSIS_BUILD_TESTS)
    add_executable(Common.t Common.cpp TestTools.cpp) # TODO
endif()
