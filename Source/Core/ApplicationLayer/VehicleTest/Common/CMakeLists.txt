#------------------------------------------------------------------------------------------
# Define some shortcuts for the sub-directory source files
#------------------------------------------------------------------------------------------
FILE(GLOB COMP_MGR_SRC ../ComponentManager/*.cpp)
FILE(GLOB COMP_PROXY_SRC ../ComponentProxy/*.cpp)
FILE(GLOB PHASE_CMDR_SRC ../PhaseCommander/*.cpp)
FILE(GLOB TEST_SEL_SRC  ../TestSelector/*.cpp)
FILE(GLOB TEST_SEQ_SRC ../TestSequencer/*.cpp)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(vehicletest STATIC VehicleTest.cpp ${COMP_MGR_SRC} ${COMP_PROXY_SRC} ${PHASE_CMDR_SRC} ${TEST_SEL_SRC} ${TEST_SEQ_SRC})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the library
#------------------------------------------------------------------------------------------
IF(INSTALL_LIBS)
	INSTALL_TARGETS(${LIB_DEPLOY_DIR}/ApplicationLayer/Vehicletest/Common vehicletest)
ENDIF(INSTALL_LIBS)

