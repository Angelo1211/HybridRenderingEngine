cmake_minimum_required(VERSION 3.7)
project(hybridRenderer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "-fPIC")

if(WIN32)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(SDL2_PATH "C:\\vs_dev_lib\\SDL2-2.0.8-MinGW")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++ (Does nothing yet)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(SDL2_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")
    set(ASSIMP_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/assimp")
  endif()
endif()


option(ENABLE_ASSIMP "using ASSIMP as importor or not" ON)


find_package(OpenMP REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} include libs)

if (ENABLE_ASSIMP)
    # set(ASSIMP_BUILD_SMD_IMPORTER OFF)
    set(ASSIMP_BUILD_AC_IMPORTER OFF)
    set(ASSIMP_BUILD_Q3D_IMPORTER OFF)
    set(ASSIMP_BUILD_CSM_IMPORTER OFF)
    set(ASSIMP_BUILD_ASSJSON_IMPORTER OFF)
    set(ASSIMP_BUILD_STEP_IMPORTER OFF)
    set(ASSIMP_BUILD_PBRT_IMPORTER OFF)
    set(ASSIMP_BUILD_3MF_IMPORTER OFF)
    set(ASSIMP_BUILD_ASSXML_IMPORTER OFF)
    add_subdirectory(external/assimp)
    set(ASSIMP_INCLUDE_DIRS "external/assimp/include" "external/assimp/build/include")
    include_directories(${ASSIMP_INCLUDE_DIRS})
    include_directories(${PROJECT_SOURCE_DIR}/external/assimp/include)
endif()

# Some useful compiler flags
#-fopt-info-vec-all -xCORE-AVX2 -no-prec-sqrt -g -ffast-math -static -static-libgcc -static-libstdc++ 

file(GLOB source_files "*.h" "*.cpp" "src/*.cpp" "include/*.h" "libs/*.h" "libs/*.c")
file(GLOB_RECURSE lib_files "libs/*.h" "libs/*.c" "libs/*.cpp")
set(ALL_SRC_FILES ${source_files} ${lib_files})
add_executable(hybridRenderer ${ALL_SRC_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
target_link_libraries(hybridRenderer ${SDL2_LIBRARY})
target_link_libraries(hybridRenderer ${ASSIMP_LIBRARY})
target_link_libraries(hybridRenderer assimp)

if(WIN32)
    set_target_properties(hybridRenderer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
    #Copy sdl2 dll file
    add_custom_command(TARGET hybridRenderer POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${SDL2_DLL}
    $<TARGET_FILE_DIR:hybridRenderer>)
    
    #Copy assimp dll
    add_custom_command(TARGET hybridRenderer POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/libs/assimp/lib64/release/assimp-vc140-mt.dll"
    $<TARGET_FILE_DIR:hybridRenderer>)
endif()
