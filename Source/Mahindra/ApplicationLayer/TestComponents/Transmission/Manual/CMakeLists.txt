#------------------------------------------------------------------------------------------
# Define the root of the Brake Test Component source tree
#------------------------------------------------------------------------------------------
SET(CUSTOMER_MANUAL_TRANS_COMP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME ManualTransTC.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} 
	ManualTransmissionTC.cpp
	ManualTransmissionTCMain.cpp)

#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})
