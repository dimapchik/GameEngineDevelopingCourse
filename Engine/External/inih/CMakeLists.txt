add_library(inih STATIC ini.c)
target_include_directories(inih PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(inih PRIVATE INI_ALLOW_MULTILINE=0)
