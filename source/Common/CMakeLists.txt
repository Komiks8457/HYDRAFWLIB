file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
add_library(Common STATIC ${SOURCES})
target_include_directories(Common PUBLIC src/)
