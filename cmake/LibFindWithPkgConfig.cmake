include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# Tries to find a library using pkg-config in a Find-Module.
#
# Input variables:
# - Prefix
# - Package (defaults to ${Prefix})
macro(FindWithPkgConfig)
    set(Prefix ${ARGV0})
    set(Package ${ARGV1})


    ### Argument deduction:

    if(NOT Package)
        set(Package ${Prefix})
    endif()


    ### Find stuff:

    pkg_check_modules(${Prefix}_PC QUIET ${Package})

    set(${Prefix}_INCLUDE_DIRS ${${Prefix}_PC_INCLUDE_DIRS})

    find_library(${Prefix}_LIBRARIES
                 NAMES ${${Prefix}_PC_LIBRARIES}
                 HINTS ${${Prefix}_PC_LIBDIR} ${${Prefix}_PC_LIBRARY_DIRS})

    set(${Prefix}_VERSION_STRING "${${Prefix}_PC_VERSION}")


    ### Process results:

    if(NOT ${Prefix}_INCLUDE_DIRS)
        set(${Prefix}_INCLUDE_DIRS ${Prefix}_INCLUDE_DIRS-NOTFOUND CACHE PATH "Path to include directory.")
    endif()

    if(NOT ${Prefix}_LIBRARIES)
        set(${Prefix}_LIBRARIES ${Prefix}_LIBRARIES-NOTFOUND CACHE FILEPATH "Path to library.")
    endif()

    find_package_handle_standard_args(${Prefix}
                                      REQUIRED_VARS ${Prefix}_INCLUDE_DIRS
                                                    ${Prefix}_LIBRARIES
                                      VERSION_VAR ${Prefix}_VERSION_STRING)
endmacro()
