cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)


project(Template)

set(CMAKE_OSX_ARCHITECTURES "arm64")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(/std:c++17)
endif()

set(CMAKE_CXX_EXTENSIONS ON)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
link_directories(${CMAKE_SOURCE_DIR}/lib)

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

find_package(Freetype REQUIRED)
message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES(/System/Library/Frameworks)
FIND_LIBRARY(COCOA_LIBRARY Cocoa)
FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
FIND_LIBRARY(IOKit_LIBRARY IOKit)
FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
set(LIBS ${LIBS} ${APPLE_LIBS})

# Set WORKSPACES to build
set(WORKSPACES
    assignment
    playground
)

set(
  assignment
  assignment_0
  assignment_1
  assignment_2
  assignment_3
  assignment_4
)

set(
  playground
  1_window
  2_triangle
  3_moving_object
  4_fractal
  5_shader
  5.1_shader_uniform
  5.2_shader_sierpinski
  6_texture_combined
  7_transformations
  8_camera
  9_model_animation
  10_skeleton_animation
)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

function(create_project_from_sources workspace demo)
	file(GLOB SOURCE
            "src/${workspace}/${demo}/*.h"
            "src/${workspace}/${demo}/*.cpp"
            "src/${workspace}/${demo}/*.vs"
            "src/${workspace}/${demo}/*.fs"
            "src/${workspace}/${demo}/*.tcs"
            "src/${workspace}/${demo}/*.tes"
            "src/${workspace}/${demo}/*.gs"
            "src/${workspace}/${demo}/*.cs"
    )
	if (demo STREQUAL "")
		SET(replaced "")
		string(REPLACE "/" "_" replaced ${workspace})
		set(NAME ${replaced})
	else()
		set(NAME "${workspace}__${demo}")
	endif()
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    if(MSVC)
		target_compile_options(${NAME} PRIVATE /std:c++17 /MP)
        target_link_options(${NAME} PUBLIC /ignore:4099)
    endif(MSVC)
    if(WIN32)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${workspace}")
        set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${workspace}/Debug")
    elseif(UNIX AND NOT APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${workspace}")
    elseif(APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${workspace}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${workspace}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${workspace}")
    endif(WIN32)
    # copy shader files to build directory
    file(GLOB SHADERS
             "src/${workspace}/${demo}/*.vs"
             # "src/${workspace}/${demo}/*.frag"
             "src/${workspace}/${demo}/*.fs"
             "src/${workspace}/${demo}/*.tcs"
             "src/${workspace}/${demo}/*.tes"
             "src/${workspace}/${demo}/*.gs"
             "src/${workspace}/${demo}/*.cs"
    )
	# copy dlls
	file(GLOB DLLS "dlls/*.dll")
    foreach(SHADER ${SHADERS})
        if(WIN32)
            # configure_file(${SHADER} "test")
            add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
			add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLLS} $<TARGET_FILE_DIR:${NAME}>)
        elseif(UNIX AND NOT APPLE)
            file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${workspace})
        elseif(APPLE)
            # create symbolic link for *.vs *.fs *.gs
            get_filename_component(SHADERNAME ${SHADER} NAME)
            makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${workspace}/${SHADERNAME} ${NAME})
        endif(WIN32)
    endforeach(SHADER)
    # if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
    if(MSVC)
        configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
    endif(MSVC)
endfunction()


# then create a project file per tutorial
foreach(WORKSPACE ${WORKSPACES})
    foreach(DEMO ${${WORKSPACE}})
		create_project_from_sources(${WORKSPACE} ${DEMO})
    endforeach(DEMO)
endforeach(WORKSPACE)

include_directories(${CMAKE_SOURCE_DIR}/includes)
