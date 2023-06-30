cmake_minimum_required(VERSION 3.13)

# update CMake module directory list
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/proj/cmake"
)

project(source
    VERSION 0.1
    DESCRIPTION "source disp"
    LANGUAGES
        C
)

# Initialize properties on all targets
set(CMAKE_CXX_STANDARD 		14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS 	OFF)

set(CMAKE_C_STANDARD 		11)
set(CMAKE_C_STANDARD_REQUIRED 	ON)
set(CMAKE_C_EXTENSIONS 		OFF)

# include(ctests)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

add_subdirectory(driver_wrappers)
add_subdirectory(hal)