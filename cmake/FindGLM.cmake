include(FindPkgConfig)
include(FindPackageHandleStandardArgs)


pkg_check_modules(GLM_PC QUIET glm)

set(GLM_INCLUDE_DIRS ${GLM_PC_INCLUDE_DIRS})
set(GLM_VERSION_STRING ${GLM_PC_VERSION})

if(NOT GLM_INCLUDE_DIRS)
    set(GLM_INCLUDE_DIRS GLM_INCLUDE_DIRS-NOTFOUND CACHE PATH "Path to include directory.")
endif()

find_package_handle_standard_args(GLM
                                  REQUIRED_VARS GLM_INCLUDE_DIRS
                                  VERSION_VAR GLM_VERSION_STRING)
