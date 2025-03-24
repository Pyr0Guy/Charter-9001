cmake_minimum_required(VERSION 3.16)
project("CHARTER-9001")

#Raylib
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VESION} QUIET)

# Dependencies
if (NOT raylib_FOUND) 

    include(FetchContent)
    
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
    set(BUILD_RAYGUI_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build raygui examples

    set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
    set(SUPPORT_MODULE_RAUDIO OFF CACHE BOOL "" FORCE)
    set(SUPPORT_SCREEN_CAPTURE OFF CACHE BOOL "" FORCE)
    set(SUPPORT_GIF_RECORDING OFF CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)

        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    endif()

endif()

if(WIN32)
    set(FMOD_PATH $ENV{FMOD_PATH})

    if(NOT FMOD_PATH)
        message(FATAL_ERROR "FMOD_PATH IS NOT SET")
    endif()

    include_directories(${FMOD_PATH}/api/core/inc/)
    find_library(FMOD_LIB NAMES fmod_vc fmodL_vc HINTS ${FMOD_PATH}/api/core/lib/x64/)
    #link_directories(${FMOD_PATH}/api/core/lib/x64/)

    include_directories(${FMOD_PATH}/api/studio/inc/)
    find_library(FMOD_STUDIO_LIB NAMES fmodstudio_vc fmodstudioL_vc HINTS ${FMOD_PATH}/api/studio/lib/x64/)
    #link_directories(${FMOD_PATH}/api/studio/lib/x64/)

endif()

if(UNIX)
	find_library(FMOD_LIB NAMES fmod HINTS /usr/lib/ /usr/lib32/ /usr/lib64/)

	if(NOT FMOD_LIB)
		message(FATAL_ERROR "FMOD NOT FOUND")
	endif()

	find_library(FMOD_STUDIO_LIB NAMES fmodstudio HINTS /usr/lib/ /usr/lib32/ /usr/lib64/)
endif()

#set(FILE_SHIT "src/main.cpp" "src/Game.cpp" "src/include/Game.cpp")
file(GLOB FILE_SHIT 
    "src/*.cpp" 
    "src/**/*.cpp"
    
    "src/include/*.hpp" 
    "src/include/**/*.hpp"
)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${FILE_SHIT})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

#I hope its temp - Pyr0Guy 11.03.2024
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/core/lib/x64/fmod_vc.lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/core/lib/x64/fmodL_vc.lib)

    file(COPY ${FMOD_PATH}/api/core/lib/x64/fmod.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
    file(COPY ${FMOD_PATH}/api/core/lib/x64/fmodL.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 

    target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/studio/lib/x64/fmodstudio_vc.lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/studio/lib/x64/fmodstudioL_vc.lib)

    file(COPY ${FMOD_PATH}/api/studio/lib/x64/fmodstudio.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
    file(COPY ${FMOD_PATH}/api/studio/lib/x64/fmodstudioL.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
endif()

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE "${FMOD_STUDIO_LIB}")
	target_link_libraries(${PROJECT_NAME} PRIVATE "${FMOD_LIB}")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp")

file(COPY "assets" DESTINATION ${CMAKE_BINARY_DIR})