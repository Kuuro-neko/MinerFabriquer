# CMake entry point
cmake_minimum_required(VERSION 3.0)
project(Tutorials)

find_package(OpenGL REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)  # Assure-toi d'être en mode Debug
set(CMAKE_CXX_FLAGS_DEBUG "-g")  # Ajoute les symboles de debug

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)")
endif ()
if (CMAKE_SOURCE_DIR MATCHES " ")
    message("Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif ()
if (CMAKE_BINARY_DIR MATCHES " ")
    message("Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif ()


# Compile external dependencies
add_subdirectory(external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if (INCLUDE_DISTRIB)
    add_subdirectory(distrib)
endif (INCLUDE_DISTRIB)


include_directories(
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glew-1.13.0/include/
        external/csv
        external/stb_image
        .
)
include_directories(${CMAKE_SOURCE_DIR}/include) # ajouté par Mathis pour éviter de mettre /external dans tous les includes

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

# main
add_executable(MinerFabriquer
        main/main.cpp
        common/shader.cpp
        common/shader.hpp
        common/imageLoader.h
        common/imageLoader.cpp
        TP/Camera/Camera.cpp
        TP/Camera/Camera_Helper.cpp
        TP/Scene/SceneNode.cpp
        TP/Scene/BlocTypes.cpp
        TP/Scene/Texture.cpp
        TP/Scene/MeshObject.cpp
        TP/Scene/VoxelChunk.cpp
        utils/GLUtils.cpp
        utils/Transform.hpp
        TP/Scene/Character.hpp
        TP/Scene/Character.cpp

        main/vertex_shader.glsl
        main/fragment_shader.glsl
)
target_link_libraries(MinerFabriquer
        ${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(MinerFabriquer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/main/")
create_target_launcher(MinerFabriquer WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/main/")

# Ensure the target directory exists
add_custom_command(
        TARGET MinerFabriquer PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/TP1
)

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*")
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$")


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode")
    add_custom_command(
            TARGET MinerFabriquer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/main/"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/TP1${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/main/"
    )
    add_custom_command(
            TARGET MinerFabriquer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/database/Blocs.csv
            ${CMAKE_BINARY_DIR}/database/Blocs.csv
    )
elseif (${CMAKE_GENERATOR} MATCHES "Xcode")

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode")