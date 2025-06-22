
cmake_minimum_required(VERSION 3.10)




project(NMKARTMOD CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(MINHOOK_PATH "C:/libs/MinHook_134_bin")
set(IMGUI_PATH "C:/libs/imgui-docking")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "src/*.inl"
)


file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS
    "${IMGUI_PATH}/imgui.cpp"
    "${IMGUI_PATH}/imgui_draw.cpp"
    "${IMGUI_PATH}/imgui_tables.cpp"
    "${IMGUI_PATH}/imgui_widgets.cpp"
    "${IMGUI_PATH}/imgui_demo.cpp"
    "${IMGUI_PATH}/backends/imgui_impl_win32.cpp"
    "${IMGUI_PATH}/backends/imgui_impl_dx11.cpp"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)


target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/SDK
    ${MINHOOK_PATH}/include
    ${IMGUI_PATH}
    ${IMGUI_PATH}/backends
)


target_compile_definitions(${PROJECT_NAME} PRIVATE 
    IMGUI_USER_CONFIG="imgui_config.h"
    IMGUI_DEFINE_MATH_OPERATORS
)


if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 8 bytes for pointer means 64-bit
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MINHOOK_PATH}/bin/MinHook.x64.lib)
    set(MINHOOK_DLL_TO_COPY ${MINHOOK_PATH}/bin/MinHook.x64.dll)
else() # otherwise 32-bit
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MINHOOK_PATH}/bin/MinHook.x86.lib)
    set(MINHOOK_DLL_TO_COPY ${MINHOOK_PATH}/bin/MinHook.x86.dll)
endif()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MINHOOK_DLL_TO_COPY}"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying MinHook DLL to output directory"
)


target_link_libraries(${PROJECT_NAME} PRIVATE d3d11 dxgi)


if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    # wd4369: enum value cannot be represented as 'unsigned char'
    # wd4309: truncation of constant value
    # wd4458: declaration of '' hides class member
    # wd4244: conversion from '' to '', possible loss of data
    # wd4267: conversion from 'size_t' to '', possible loss of data
    target_compile_options(${PROJECT_NAME} PRIVATE 
        "/wd4369" "/wd4309" "/wd4458" "/wd4244" "/wd4267"
    ) 
    

    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
    # set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
endif()

# optional: set output directory for the dll
# makes it easier to find the compiled dll
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # for dlls place in bin for convenience
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # for executables (and DLLs on Windows)
