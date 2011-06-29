#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME InputServer)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Input server headers
#------------------------------------------------------------------------------------------
SET(CORE_INPUT_SERVER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE PATH
        "Path to the core input server headers" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the library
#------------------------------------------------------------------------------------------
SET(CORE_INPUT_SERVER_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
        "Path to the core input server library" FORCE)

#------------------------------------------------------------------------------------------
# Add the InputServer Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libInputDevices.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Common)

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All Input Server Devices link against libInputDevicelib.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(InputDevicelib)

#------------------------------------------------------------------------------------------
# Build the executables after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CLV295ScannerMgr)
ADD_SUBDIRECTORY(MapleKeypadManager)
ADD_SUBDIRECTORY(PendantManager)
ADD_SUBDIRECTORY(SymbolScannerMgr)
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC InputServer.cpp)

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

