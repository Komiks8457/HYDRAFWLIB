include (CMakeParseArguments)

function(generate_product_version outfiles)
    set (oneValueArgs
            VERSION_MAJOR
            VERSION_MINOR
            VERSION_PATCH
            NAME
            BUNDLE
            ICON
            COMMENTS
            COMPANY_NAME
            COMPANY_COPYRIGHT
            ORIGINAL_FILENAME
            INTERNAL_NAME
            FILE_DESCRIPTION
    )
    cmake_parse_arguments(PRODUCT "" "${oneValueArgs}" "" ${ARGN})

    # 1. Date Logic for 1.DD.MM.YY
    string(TIMESTAMP CURRENT_DAY "%d")
    string(TIMESTAMP CURRENT_MONTH "%m")
    string(TIMESTAMP CURRENT_YEAR "%y")
    string(TIMESTAMP CURRENT_HOUR "%H")
    string(TIMESTAMP CURRENT_MIN "%M")
    string(TIMESTAMP CURRENT_SEC "%S")

    math(EXPR CURRENT_DAY "${CURRENT_DAY} + 0")
    math(EXPR CURRENT_MONTH "${CURRENT_MONTH} + 0")

    set(PRODUCT_VERSION_MAJOR "${PRODUCT_VERSION_MAJOR}")
    if("${PRODUCT_VERSION_MAJOR}" STREQUAL "")
        set(PRODUCT_VERSION_MAJOR 1)
    endif()

    set(PRODUCT_VERSION_MINOR "${PRODUCT_VERSION_MINOR}")
    if("${PRODUCT_VERSION_MINOR}" STREQUAL "")
        set(PRODUCT_VERSION_MINOR 0)
    endif()

    set(PRODUCT_VERSION_PATCH "${PRODUCT_VERSION_PATCH}")
    if("${PRODUCT_VERSION_PATCH}" STREQUAL "")
        set(PRODUCT_VERSION_PATCH 0)
    endif()

    set(PRODUCT_VERSION_BUILD    ${CURRENT_DAY}${CURRENT_MONTH}${CURRENT_YEAR})
    set(PRODUCT_VERSION_REVISION ${CURRENT_HOUR}${CURRENT_MIN}${CURRENT_SEC})

    # 2. Map Metadata Strings
    set(PRODUCT_COMMENTS           "${PRODUCT_COMMENTS}")
    set(PRODUCT_COMPANY_NAME       "${PRODUCT_COMPANY_NAME}")
    set(PRODUCT_COMPANY_COPYRIGHT  "${PRODUCT_COMPANY_COPYRIGHT}")
    set(PRODUCT_FILE_DESCRIPTION   "${PRODUCT_FILE_DESCRIPTION}")
    set(PRODUCT_INTERNAL_NAME      "${PRODUCT_INTERNAL_NAME}")
    set(PRODUCT_ORIGINAL_FILENAME  "${PRODUCT_ORIGINAL_FILENAME}")
    set(PRODUCT_BUNDLE             "${PRODUCT_BUNDLE}")

    # 3. Path Logic (FORCING SOURCE DIRECTORY)
    set(_InFile   "${CMAKE_CURRENT_SOURCE_DIR}/version/VersionInfo.in")
    set(_RcInFile "${CMAKE_CURRENT_SOURCE_DIR}/version/VersionResource.in")

    set(_OutHFile  "${CMAKE_SOURCE_DIR}/global/others/version.h")
    set(_OutRcFile "${CMAKE_CURRENT_SOURCE_DIR}/version/VersionResource.rc")

    # 4. Generate the files
    configure_file("${_InFile}" "${_OutHFile}" @ONLY)
    configure_file("${_RcInFile}" "${_OutRcFile}" @ONLY)

    # Return the .rc path to the caller so it can be added to the target
    set(${outfiles} "${_OutRcFile}" PARENT_SCOPE)
endfunction()