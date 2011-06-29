#------------------------------------------------------------------------------------------
# Set our compiler/linker options
#------------------------------------------------------------------------------------------

# Read environment variable values
SET(ENV_CFLAGS $ENV{CFLAGS})
SET(ENV_CCFLAGS $ENV{CCFLAGS})
SET(ENV_LDFLAGS $ENV{LDFLAGS})

# Set default options if no environment variables exist
IF(NOT ENV_CFLAGS)
    SET(CFLAGS "-Wall -Y _cpp -g")
    #MESSAGE(STATUS "Setting default CFLAGS options: <${CFLAGS}>")
ELSE(NOT ENV_CFLAGS)
    SET(CFLAGS ${ENV_CFLAGS})
    MESSAGE(STATUS "Using CFLAGS environment var: <${CFLAGS}>")
ENDIF(NOT ENV_CFLAGS)

IF(NOT ENV_CCFLAGS)
    SET(CCFLAGS ${CFLAGS})
    #MESSAGE(STATUS "Setting default CCFLAGS options: <${CCFLAGS}>")
ELSE(NOT ENV_CCFLAGS)
    SET(CCFLAGS ${ENV_CCFLAGS})
    MESSAGE(STATUS "Using CCFLAGS environment var: <${CCFLAGS}>")
ENDIF(NOT ENV_CCFLAGS)

IF(NOT ENV_LDFLAGS)
    SET(LDFLAGS "-Y _cpp -lcpp -lsocket -lang-c++")
    #MESSAGE(STATUS "Setting default LDFLAGS options: <${LDFLAGS}>")
ELSE(NOT ENV_LDFLAGS)
    SET(LDFLAGS ${ENV_LDFLAGS})
    MESSAGE(STATUS "Using LDFLAGS environment var: <${LDFLAGS}>")
ENDIF(NOT ENV_LDFLAGS)

#------------------------------------------------------------------------------------------
# Put C Compiler options into the Cache file
#------------------------------------------------------------------------------------------
SET(CMAKE_C_FLAGS
    "${CFLAGS}"
    CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE)
SET(CMAKE_C_FLAGS_DEBUG
    "${CFLAGS} -g"
    CACHE STRING
    "Flags used by the compiler during debug builds."
    FORCE)
SET(CMAKE_C_FLAGS_MINSIZEREL
    "${CFLAGS} -Os"
    CACHE STRING
    "Flags used by the compiler during release minsize builds."
    FORCE)
SET(CMAKE_C_FLAGS_RELEASE
    "${CFLAGS} -O3"
    CACHE STRING
    "Flags used by the compiler during release builds."
    FORCE)
SET(CMAKE_C_FLAGS_RELWITHDEBINFO
    "${CFLAGS} -O2"
    CACHE STRING
    "Flags used by the compiler during Release with Debug builds."
    FORCE)
#------------------------------------------------------------------------------------------
# Put C++ Compiler options into the Cache file
#------------------------------------------------------------------------------------------
SET(CMAKE_CXX_FLAGS 
    "${CCFLAGS}"
    CACHE STRING
    "Flags used by the compiler during all build types."
    FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG
    "${CCFLAGS} -g"
    CACHE STRING
    "Flags used by the compiler during debug builds."
    FORCE)
SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "${CCFLAGS} -Os"
    CACHE STRING
    "Flags used by the compiler during release minsize builds."
    FORCE)
SET(CMAKE_CXX_FLAGS_RELEASE
    "${CCFLAGS} -O3"
    CACHE STRING
    "Flags used by the compiler during release builds."
    FORCE)
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "${CCFLAGS} -O2"
    CACHE STRING
    "Flags used by the compiler during Release with Debug builds."
    FORCE)
#------------------------------------------------------------------------------------------
# Put Linker options into the Cache file
#------------------------------------------------------------------------------------------
SET(CMAKE_EXE_LINKER_FLAGS
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker."
    FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG
    "${LDFLAGS} -g"
    CACHE STRING
    "Flags used by the linker during debug builds."
    FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker during release minsize builds."
    FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker during release builds."
    FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker during Release with Debug builds."
    FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG
    "${LDFLAGS} -g"
    CACHE STRING
    "Flags used by the linker during debug builds."
    FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker during release minsize builds."
    FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE
    "${LDFLAGS}"
    CACHE STRING
    "Flags used by the linker during release builds."
    FORCE)
SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
    "${LDFLAGS} -g"
    CACHE STRING
    "Flags used by the linker during Release with Debug builds."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS
    "${LDFLAGS} -shared"
    CACHE STRING
    "Flags used by the linker during the creation of dll's."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG:STRING=-Y _cpp
    "${LDFLAGS} -shared"
    CACHE STRING
    "Flags used by the linker during debug builds."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING=-Y _cpp
    "${LDFLAGS} -shared"
    CACHE STRING
    "Flags used by the linker during release minsize builds."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING=-Y _cpp
    "${LDFLAGS} -shared"
    CACHE STRING
    "Flags used by the linker during release builds."
    FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING=-Y _cpp
    "${LDFLAGS} -shared"
    CACHE STRING
    "Flags used by the linker during Release with Debug builds."
    FORCE)
