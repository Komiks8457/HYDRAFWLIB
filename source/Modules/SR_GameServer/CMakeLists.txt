file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
add_library(GameServer STATIC ${SOURCES})
target_link_libraries(GameServer HydraFramework)
target_include_directories(GameServer PUBLIC src/)
