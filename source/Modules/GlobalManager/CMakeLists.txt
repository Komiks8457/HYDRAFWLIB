file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
add_library(GlobalManager STATIC ${SOURCES})
target_link_libraries(GlobalManager HydraFramework)
target_include_directories(GlobalManager PUBLIC src/)
