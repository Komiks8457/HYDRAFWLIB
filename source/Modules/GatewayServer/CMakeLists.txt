file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
add_library(GatewayServer STATIC ${SOURCES})
target_link_libraries(GatewayServer HydraFramework)
target_include_directories(GatewayServer PUBLIC src/)
