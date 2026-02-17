set(FONT_DIR "${CMAKE_SOURCE_DIR}/assets/fonts")
set(FONT_FILE "${FONT_DIR}/JetBrainsMono-Regular.ttf")
set(FONT_ZIP "${FONT_DIR}/JetBrainsMono.zip")
set(FONT_URL "https://github.com/JetBrains/JetBrainsMono/releases/download/v2.304/JetBrainsMono-2.304.zip")

if(NOT EXISTS "${FONT_FILE}")
    message(STATUS "Downloading JetBrains Mono font...")
    file(DOWNLOAD "${FONT_URL}" "${FONT_ZIP}"
         STATUS DOWNLOAD_STATUS
         SHOW_PROGRESS)
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download font: ${DOWNLOAD_STATUS}")
    endif()

    file(ARCHIVE_EXTRACT INPUT "${FONT_ZIP}" DESTINATION "${FONT_DIR}")

    # Move the TTF file to the expected location
    file(GLOB FONT_FILES "${FONT_DIR}/fonts/ttf/JetBrainsMono-Regular.ttf")
    if(FONT_FILES)
        list(GET FONT_FILES 0 FOUND_FONT)
        file(COPY "${FOUND_FONT}" DESTINATION "${FONT_DIR}")
    endif()

    # Clean up extracted directories and zip
    file(REMOVE "${FONT_ZIP}")
    file(REMOVE_RECURSE "${FONT_DIR}/fonts")

    if(NOT EXISTS "${FONT_FILE}")
        message(FATAL_ERROR "Font file not found after extraction: ${FONT_FILE}")
    endif()
    message(STATUS "Font downloaded successfully: ${FONT_FILE}")
else()
    message(STATUS "Font already exists: ${FONT_FILE}")
endif()
