#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME TeslaTestResultServer.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------
# Add the core TestResultServer include directory
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_TEST_RESULT_SERVER_INC_DIR})

#------------------------------------------------------------------------------
# Define a cache variable to hold the path to the TataTestResultServer headers
#------------------------------------------------------------------------------
SET(CUSTOMER_TEST_RESULT_SERVER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH
        "Path to the customer test result server includes" FORCE)
    
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------
# Create a library for this class.
#------------------------------------------------------------------------------
ADD_LIBRARY(teslatestresultserverlib STATIC TeslaTestResultServer.cpp)
    
#------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------
#ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} TeslaTestResultServerMain.cpp TeslaTestResultServer.cpp)

#------------------------------------------------------------------------------
# All SystemMonitor processes link against libsystemmonitor.a
#------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${EXE_NAME} testresultserverlib)

#------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
