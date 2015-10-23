file(GLOB Sources *.h *.c)
add_library(lua-cjson STATIC ${Sources})
