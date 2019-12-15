cmake_minimum_required(VERSION 3.16.1)

project(CLGLInterop)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

find_package(glfw3 3.3 REQUIRED)
find_package(OpenCL 1.2 REQUIRED)
find_package(OpenGL 3.3 REQUIRED)

if(APPLE)
    find_package(X11 REQUIRED)
    if(X11_FOUND AND NOT TARGET X11::x11)
        add_library(X11::x11 UNKNOWN IMPORTED)
        set_target_properties(X11::x11 PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGE "C"
            IMPORTED_LOCATION "${X11_X11_LIB}"
            INTEFACE_INCLUDE_DIRECTORIES "${X11_INCLUDE_DIR}")
    endif()
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
         "${CMAKE_INSTALL_PREFIX}" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
    endif("${isSystemDir}" STREQUAL "-1")
endif()

add_subdirectory(external_sources/glad)

file(GLOB common_src "common/*.cpp")
file(GLOB examples_src "examples/*.cpp")

macro(BUILD_EXAMPLE EXAMPLE_NAME EXAMPLE_SOURCE OUT_DIR_NAME)
    add_executable(${EXAMPLE_NAME}
        ${EXAMPLE_SOURCE}
        ${common_src}
        )
    target_include_directories(${EXAMPLE_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}
        )
    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE
        glfw
        OpenCL::OpenCL
        OpenGL::GL
        ${CMAKE_DL_LIBS}
        glad-interface
        $<$<PLATFORM_ID:Darwin>:X11::x11>
        )
    target_compile_definitions(${EXAMPLE_NAME}
        PRIVATE
        "ASSETS_DIR=\"${ASSETS_DIR}\""
        $<$<PLATFORM_ID:Linux>:OS_LNX>
        $<$<PLATFORM_ID:DarWin>:OS_MAC>
        $<$<PLATFORM_ID:Windows>:OS_WIN>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        )
    set_target_properties(${EXAMPLE_NAME}
        PROPERTIES
        OUTPUT_NAME ${EXAMPLE_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR_NAME}
        FOLDER "CLGL-Examples"
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )
endmacro()

foreach(FILE ${examples_src})
    get_filename_component(EXAMPLE ${FILE} NAME_WE)
    get_filename_component(FULL_DIR_NAME ${FILE} PATH)
    get_filename_component(DIR_NAME ${FULL_DIR_NAME} NAME)
    BUILD_EXAMPLE(${EXAMPLE} ${FILE} ${DIR_NAME})
endforeach()
