cmake_minimum_required(VERSION 3.7)
project(hybridRenderer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

if(WIN32)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(SDL2_PATH "C:\\vs_dev_lib\\SDL2-2.0.8-MinGW")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++ (Does nothing yet)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(SDL2_PATH "C:\\vs_dev_lib\\SDL2-2.0.8-MSVC")
    set(ASSIMP_PATH "C:\\vs_dev_lib\\ASSIMP-MSVC")
  endif()
endif()
# Picking correct sdl2 lib for each compiler for the windwos version


find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} include libs)

find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR} )

# Some useful compiler flags
#-fopt-info-vec-all -xCORE-AVX2 -no-prec-sqrt -g -ffast-math -static -static-libgcc -static-libstdc++ 

file(GLOB source_files "*.h" "*.cpp" "src/*.cpp" "include/*.h" "libs/*.h" "libs/*.c")
file(GLOB_RECURSE lib_files "libs/*.h" "libs/*.c")
set(ALL_SRC_FILES ${source_files} ${lib_files})
add_executable(hybridRenderer ${ALL_SRC_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
target_link_libraries(hybridRenderer ${SDL2_LIBRARY})
target_link_libraries(hybridRenderer ${ASSIMP_LIBRARY})

if(WIN32)
    add_custom_command(TARGET hybridRenderer POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${SDL2_DLL}
    $<TARGET_FILE_DIR:hybridRenderer>)
endif()