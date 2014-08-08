#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ToyotaMachineDataBroker.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------------------
# Need to add core include directory
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVERS_DIR}/MachineDataBroker)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Define the source files needed to build the executable
#------------------------------------------------------------------------------------------
FILE(GLOB TOYOTA_MDB_SOURCES *.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} ${TOYOTA_MDB_SOURCES})
CREATE_QXE(${EXE_NAME} ${TOYOTA_MDB_SOURCES})
TARGET_LINK_LIBRARIES(${EXE_NAME} m -lds -lm)

#------------------------------------------------------------------------------------------
# Link against the core lib
#------------------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${EXE_NAME} machinedatabrokerlib)

#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${CUSTOMER_SERVICES_BIN_DEPLOY_DIR} ${EXE_NAME})
