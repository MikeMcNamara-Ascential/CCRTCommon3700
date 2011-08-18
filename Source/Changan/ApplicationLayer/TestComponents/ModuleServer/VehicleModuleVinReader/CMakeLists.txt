#------------------------------------------------------------------------------------------
# Create a list of all the cpp files in this directory needed to create the executable
#------------------------------------------------------------------------------------------
SET(VEHICLE_MODULE_VIN_READER_SOURCES VehicleModuleVinReader.cpp)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME VehicleModuleVinReader)

#------------------------------------------------------------------------------------------
# Add the Customer InputServer Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_INPUT_SERVER_LIB_DIR} ${CORE_INPUT_SERVER_INC_DIR})
 
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
ADD_LIBRARY(${LIB_NAME} STATIC ${VEHICLE_MODULE_VIN_READER_SOURCES})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
#ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})
