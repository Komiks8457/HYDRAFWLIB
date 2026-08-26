include(version/VersionInfo.cmake)

generate_product_version(VersionInfo
        VERSION_MAJOR       1
        FILE_DESCRIPTION    "Hydra Framework"
        COMPANY_NAME        "8ATHALA Co., Ltd."
        COMPANY_COPYRIGHT   "Copyright (C) 2026"
        ORIGINAL_FILENAME   "HYDRA.DLL"
        INTERNAL_NAME       "HYDRA"
)

set(RESOURCE_FILES
        "src/Hydra.cpp"
        "src/Hydra.def"
        "src/Library.cpp"
        "src/StaticPatches.cpp"
)

add_library(HydraDLL SHARED ${RESOURCE_FILES} ${VersionInfo})

target_link_libraries(HydraDLL
        HydraFramework
        AgentServer
        DownloadServer
        FarmManager
        GatewayServer
        GlobalManager
        MachineManager
        GameServer
        ShardManager
)

target_compile_definitions(HydraDLL PRIVATE SERVER_BUILD)

if(MSVC)
    set_target_properties(HydraDLL
            PROPERTIES
            LINK_FLAGS "/DEF:\"src/Hydra.def\""
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/Server"
            OUTPUT_NAME "Hydra"
    )
endif()

add_custom_command(TARGET HydraDLL
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/hydrafwlib.dll"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/ASGS/AgentServer1/hydrafwlib.dll"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/ASGS/GameServer1/hydrafwlib.dll"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/ASGS/GameServer2/hydrafwlib.dll"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/ASGS/GameServer3/hydrafwlib.dll"
        #COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:HydraDLL> "H:/silkroadr/SMC/hydrafwlib.dll"
)