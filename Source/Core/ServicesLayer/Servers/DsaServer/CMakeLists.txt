#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME DsaServer)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libdsaserver.a library
#------------------------------------------------------------------------------------------
SET(CORE_DSA_SERVER_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
        "Path to the core dsa server library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the dsa server headers
#------------------------------------------------------------------------------------------
SET(CORE_DSA_SERVER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH
        "Path to the core dsa server headers" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC DsaIn.cpp DsaOut.cpp ${EXE_NAME}.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME})

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
