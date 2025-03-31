cmake_minimum_required(VERSION 3.16)
project("CHARTER-9001")

set(IS_RELEASE_BUILD OFF)
set(IS_DEBUG_BUILD OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(IS_RELEASE_BUILD ON)
else()
    set(IS_DEBUG_BUILD ON)
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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
    set(raylib_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)

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
    include_directories(${FMOD_PATH}/api/studio/inc/)

    if(IS_RELEASE_BUILD)
        find_library(FMOD_LIB NAMES fmod_vc HINTS ${FMOD_PATH}/api/core/lib/x64/)
        find_library(FMOD_STUDIO_LIB NAMES fmodstudio_vc HINTS ${FMOD_PATH}/api/studio/lib/x64/)
    else()
        find_library(FMOD_LIB NAMES fmodL_vc HINTS ${FMOD_PATH}/api/core/lib/x64/)
        find_library(FMOD_STUDIO_LIB NAMES fmodstudioL_vc HINTS ${FMOD_PATH}/api/studio/lib/x64/)
    endif()
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

target_link_libraries(${PROJECT_NAME} PRIVATE raylib glfw)

#I hope its temp - Pyr0Guy 11.03.2024
if(WIN32)
    if(IS_RELEASE_BUILD)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/core/lib/x64/fmod_vc.lib)
        file(COPY ${FMOD_PATH}/api/core/lib/x64/fmod.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
        
        target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/studio/lib/x64/fmodstudio_vc.lib)
        file(COPY ${FMOD_PATH}/api/studio/lib/x64/fmodstudio.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/core/lib/x64/fmodL_vc.lib)
        file(COPY ${FMOD_PATH}/api/core/lib/x64/fmodL.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
        
        target_link_libraries(${PROJECT_NAME} PRIVATE ${FMOD_PATH}/api/studio/lib/x64/fmodstudioL_vc.lib)
        file(COPY ${FMOD_PATH}/api/studio/lib/x64/fmodstudioL.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
    endif()


endif()

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE "${FMOD_STUDIO_LIB}")
	target_link_libraries(${PROJECT_NAME} PRIVATE "${FMOD_LIB}")
endif()

if(MSVC AND IS_RELEASE_BUILD)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-mwindows")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_RELEASE_BUILD=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_DEBUG_BUILD=1)
endif()

file(COPY "assets" DESTINATION ${CMAKE_BINARY_DIR})