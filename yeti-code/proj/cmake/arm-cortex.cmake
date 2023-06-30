set(CMAKE_SYSTEM_NAME          Generic)   # "Generic" means bare metal
set(CMAKE_SYSTEM_PROCESSOR     ARM)

# Define the compilers
set(CMAKE_C_COMPILER          arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER        arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER        ${CMAKE_C_COMPILER})




#   definition of definitions
#
#    ARCH_FLAGS                      ARCHITECTURE
#    WARNING_FLAGS
#    DEFINITIONS_FLAGS               #DEFS
#
#    CMAKE_ASM_FLAGS_INIT          ASSEMBLY FILES,          = ARCH + SPECIFIC
#    CMAKE_C_FLAGS_INIT            GCC COMPILE C FILES.     = ARCH + WARNS  + SPECIFIC  + DEFS.
#    CMAKE_CXX_FLAGS_INIT          G++ COMPILE CPP FILES.   = ARCH + WARNS  + SPECIFIC  + DEFS.              NONE EXIST IN THIS PROJECT
#    CMAKE_EXE_LINKER_FLAGS_INIT   LINKER.                  = ARCH + SPECIFIC.                     OUTPUT STILL HAS WARNS AND DEFS !!



#  GOOD
string( JOIN " " ARCH_FLAGS
   -g3 
   -gdwarf-2 
   -mcpu=cortex-m33 
   -mthumb 
   -mfpu=fpv5-sp-d16  
   -mfloat-abi=hard  
)


#    TEMPORARY
string(JOIN " " WARNING_FLAGS
#    -pedantic              # disable compiler extensions
    -Wall                  # standard warnings
    -Wextra                # more warnings
#    -Wtrigraphs
#    -Wparentheses          # missing parentheses
#    -Wcast-align           # pointer cast increases alignment
#    -Wcast-qual            # casting away a type qualifier
#    -Wuninitialized        # uninitialized local variable
#    -Wsign-compare         # comparison between signed and unsigned values
#    -Wshadow               # symbol name shadows another symbol
#    -Wmissing-declarations # global function defined without previous declaration
#    -Wfatal-errors
)



#  GOOD
# Use this structure/string to place #defines.  
# Remember to put a '-D' at the beginning of the string.
# These will be placed in 'C' files as well as 'C++', but not assembly files.
string( JOIN " " DEFINITIONS_FLAGS
   -DDEBUG_EFM=1  
   -DEFM32PG22C200F64IM40=1 
   -DSL_COMPONENT_CATALOG_PRESENT=1 
)


# GOOD
# Initialize assembly flags
string(JOIN " " CMAKE_ASM_FLAGS_INIT
    ${ARCH_FLAGS}  
   -imacrossl_gcc_preinclude.h   
   -I"/dirDockerProjAlias/gecko_sdk_4.0.2/platform/common/toolchain/inc"         # FOR PREINCLUDE...
#   -c   #  not needed.  do not include here.
   -x assembler-with-cpp  
)


#  GOOD
# Initialize C flags
string(JOIN " " CMAKE_C_FLAGS_INIT
    ${ARCH_FLAGS}  
    ${WARNING_FLAGS}
    ### extra directives for C
   -std=c99 
   -Os
   -fno-builtin 
   -ffunction-sections 
   -fdata-sections 
   -imacrossl_gcc_preinclude.h 
   -I"/dirDockerProjAlias/gecko_sdk_4.0.2/platform/common/toolchain/inc"        # FOR PREINCLUDE...
#   -c     # REMOVING KEEPS IT FROM GOING TO THE LINKER, WHERE IT DISRUPTS THE PROCESS.
   -fmessage-length=0  
   -MMD  
   -MP   
#    -Wno-switch
    ${DEFINITIONS_FLAGS}
)

# Initialize C++ flags
string(JOIN " " CMAKE_CXX_FLAGS_INIT
    ${ARCH_FLAGS}  
    ${WARNING_FLAGS}
    -fmessage-length=0
    -fmerge-constants
    ${DEFINITIONS_FLAGS}
)

# Initialize linker flags
#
# NOTE::::  THIS STRING IS USED FOR LINKER, BUT WHEN LINKER FINALLY GETS THE FLAGS,
#         IT INCLUDES THE STRING   CMAKE_C_FLAGS_INIT   INCORRECTLY.
#         NEED TO RESOLVE WHY THIS HAPPENS AND AVOID IT, AS THE FLAGS MAY CONFLICT.
string(JOIN " " CMAKE_EXE_LINKER_FLAGS_INIT
    ${ARCH_FLAGS}
    --specs=nano.specs                # Link small standard library         good
    -Xlinker --gc-sections   # Enable garbage collection of unused input sections   good
    -lgcc 
    -lc 
    -lm 
    -lnosys 	
)

# Don't try to run *target* programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM    NEVER)

# Don't try to build *host* libraries
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY    ONLY)

# Don't try to include *host* headers
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE    ONLY)

# When doing a test build to check the compilers, don't try to execute it on host.
set(CMAKE_TRY_COMPILE_TARGET_TYPE        STATIC_LIBRARY)
