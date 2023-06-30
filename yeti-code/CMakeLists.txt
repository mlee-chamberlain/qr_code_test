cmake_minimum_required(VERSION 3.13)

# update CMake module directory list
# this is in the top-level cmake.
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/proj/cmake"
)


# you are using assembly, need to activate it before the project is defined.
enable_language ( ASM )

project(yeti-display
    VERSION 0.1
    DESCRIPTION "This project defines the yeti-display binary hosted on an EFM32PG22 microprocessor node."
    LANGUAGES
        C
        CXX
)



message ("IN THE MAIN PROJECT CMAKE FILE.   INVOKING THE BASIC CONFIGURATION....................................")
#  set(  YOURNAMEHERE   YES)




#  THESE ARE IN THE PROJ/CMAKE DIRECTORY.
#    THESE SET THE LINKER AND MAP-FILE GENERATION AND #DEFS.
#    THESE CASCADE TO ALL PROJECT DEPENDENCIES.
include(linker-script)
include(map-file)
include(compile_warnings)


add_executable(${PROJECT_NAME}.axf
    main.c
    app.c
)


#  THIS IS NOT IN THE COMMAND SECTION.    MUST BE A SCRIPT
custom_target_linker_script (${PROJECT_NAME}.axf
    autogen/linkerfile.ld
)


#  THIS IS NOT IN THE COMMAND SECTION. 
custom_target_map_file  (${PROJECT_NAME}.axf)



# Initialize properties on all targets
# top level cmake.
set(CMAKE_CXX_STANDARD 		14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS 	OFF)

set(CMAKE_C_STANDARD 		11)
set(CMAKE_C_STANDARD_REQUIRED 	ON)
set(CMAKE_C_EXTENSIONS 		OFF)


# what is this?  I dunno.
# include(ctests)


# this is needed for all the submodules to find project-build.h
# this can probs go in the logic subdir.  pb.h only used in there.
# directories added to the directory property of the cmake file, 
# and to the target property for each target in the list.
# Can append or prepend by the AFTER or BEFORE designator.
#  in autogen... include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})



#  ADD SUBDIRECTORY TO THE BUILD.  
#  EXPECT A CMAKE LIST IN EACH OF THESE.
#  PROCESSED IN ORDER BEFORE CONTINUING.

add_subdirectory(gecko_sdk_4.0.2)
add_subdirectory(source)


# SPECIFY LIBRARIES AND FLAGS TO USE WHEN LINKING TARGET.
# TARGET IS FIRST, A PREVIOUSLY DEFINED EXE OR LIB [ADD_].
# FULL PATH TO SOURCE, OR A LIB NAME.
# PRIV/PUB/INTERFACE CAN BE DESIGNATED FOR EACH ITEM.
# LINKS ARE TRANSITIVE, LINKED LIBS IN ITEMS LINK HERE TOO.
#
# NOTE::: THIS IS HOW TO LINK LIBRARIES IN THE PROJECT.
#        HOWEVER, THESE LIBRARY DEPENDENCIES ALSO INCLUDE TARGET LIBRARIES.
#        THIS CAUSES LIBRARIES TO BE LISTED MULTIPLE TIMES IN THE LINKING COMMAND.
#        UNKNOWN IF THIS CAUSES DETRIMENTAL EFFECTS ON THE OUTPUT.
#        ALL WE REALLY NEED FROM THE DEPENDENCIES IS THE HEADER FILES SO THEY HAVE THE PROPER PROTOTYPES.
target_link_libraries(${PROJECT_NAME}.axf
    PRIVATE
         gecko-sdk
         driver_wrappers
         hal
)





# CUSTOM OUTPUTS.
# THIS IS ONE OF THE TWO SPECIFIC SIGNATURES FOR THIS COMMAND.
# ASSOCIATE WITH A PREVIOUSLY DEFINED TARGET, DEFINED IN HERE.
add_custom_command(TARGET ${PROJECT_NAME}.axf
    POST_BUILD
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.axf
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.axf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.axf ${PROJECT_NAME}.hex
)



# this is a good place for this declaration.
# turn it off when you don't need if
set(CMAKE_VERBOSE_MAKEFILE       OFF )
