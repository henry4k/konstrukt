# Handle platforms missing isinf() macro (Eg, some Solaris systems).
include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(isinf math.h HAVE_ISINF)
if(NOT HAVE_ISINF)
    add_definitions(-DUSE_INTERNAL_ISINF)
endif()

if(WIN32)
    # Windows sprintf()/strtod() handle NaN/inf differently. Not supported.
    add_definitions(-DDISABLE_INVALID_NUMBERS)
endif()

file(GLOB Sources *.h *.c)
add_library(lua-cjson STATIC ${Sources})
