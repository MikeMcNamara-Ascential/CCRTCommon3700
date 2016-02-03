#------------------------------------------------------------------------------------------
# Create a list of all the cpp files in this directory needed to create the executable
#------------------------------------------------------------------------------------------
SET(VINSTAMPERMANAGER_SOURCES VINStamperMgr.cpp)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME VINStamperMgr)

#------------------------------------------------------------------------------------------
# All Input Server Devices link against libInputDevicelib.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(InputDevicelib)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC ${VINSTAMPERMANAGER_SOURCES})



#------------------------------------------------------------------------------------------
# Add the InputServer Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Include
${CORE_SERVERS_DIR}/InputServer/Include)

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


