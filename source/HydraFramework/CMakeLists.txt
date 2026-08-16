file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
add_library(HydraFramework STATIC ${SOURCES})
target_link_libraries(HydraFramework Common)
target_include_directories(HydraFramework PUBLIC src/)
