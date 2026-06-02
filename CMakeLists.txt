cmake_minimum_required(VERSION 3.20)
project(ParametricModelling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Maya SDK ──────────────────────────────────────────────────────────────────
# Override with: cmake -DMAYA_LOCATION="C:/Program Files/Autodesk/MayaXXXX" ...
set(MAYA_LOCATION "C:/Program Files/Maya_SDK/devkitBase"
    CACHE PATH "Maya DevKit directory")

set(MAYA_INCLUDE_DIR "${MAYA_LOCATION}/include")
set(MAYA_LIBRARY_DIR "${MAYA_LOCATION}/lib")

# ── Maya plug-in ──────────────────────────────────────────────────────────────
add_library(ParametricStaircaseNode SHARED
    src/pluginMain.cpp
    src/parametricStaircaseNode.cpp
    src/parametricBridgeNode.cpp
    src/staircaseMesh.cpp
    src/bridgeMesh.cpp
)

target_include_directories(ParametricStaircaseNode PRIVATE
    "${MAYA_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(ParametricStaircaseNode PRIVATE
    "${MAYA_LIBRARY_DIR}/Foundation.lib"
    "${MAYA_LIBRARY_DIR}/OpenMaya.lib"
    "${MAYA_LIBRARY_DIR}/OpenMayaUI.lib"
)

target_compile_definitions(ParametricStaircaseNode PRIVATE
    NT_PLUGIN
    REQUIRE_IOSTREAM
)

# Maya requires the multi-threaded DLL runtime (/MD, not /MT or /MTd)
if(MSVC)
    target_compile_options(ParametricStaircaseNode PRIVATE /MD)
endif()

# Maya plug-ins on Windows use the .mll extension with no lib prefix
set_target_properties(ParametricStaircaseNode PROPERTIES
    SUFFIX ".mll"
    PREFIX ""
)
