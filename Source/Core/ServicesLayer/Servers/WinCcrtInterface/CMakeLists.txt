#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME WinCcrtInterface)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libvehicledatabroker.a library
#------------------------------------------------------------------------------------------
SET(CORE_DATA_SERVER_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
        "Path to the core data server library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the vehicle data broker headers
#------------------------------------------------------------------------------------------
SET(CORE_DATA_SERVER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH
        "Path to the core data server headers" FORCE)

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
ADD_LIBRARY(${LIB_NAME} STATIC ${EXE_NAME}.cpp)

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


#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
#SET(EXE_NAME WinCcrtInterface)
#
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
#ADD_SUBDIRECTORY(Configuration)
#
#------------------------------------------------------------------------------------------
# Where to find the CoreId.h file
#------------------------------------------------------------------------------------------
#INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Include)
#
#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
#STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
#SET(LIB_NAME ${TMP_LIB_NAME}lib)
#ADD_LIBRARY(${LIB_NAME} STATIC WinCcrtInterface.cpp )
#
#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
#CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)
#TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME} -lds -lm)
#
#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
#ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)
#
#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
#INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})

