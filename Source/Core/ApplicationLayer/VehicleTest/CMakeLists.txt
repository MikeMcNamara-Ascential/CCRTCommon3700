#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libVehicleTest.a library
#------------------------------------------------------------------------------------------
SET(CORE_VEHICLE_TEST_LIB_PATHS 
        ${CMAKE_CURRENT_BINARY_DIR}/ComponentManager
        ${CMAKE_CURRENT_BINARY_DIR}/ComponentProxy
        ${CMAKE_CURRENT_BINARY_DIR}/PhaseCommander
        ${CMAKE_CURRENT_BINARY_DIR}/TestSelector
        ${CMAKE_CURRENT_BINARY_DIR}/TestSequencer
        ${CMAKE_CURRENT_BINARY_DIR}/Library)
SET(CORE_VEHICLE_TEST_LIB_PATH ${CORE_VEHICLE_TEST_LIB_PATHS} CACHE STRING
        "Path(s) to the core vehicle test libraries" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the t-case component headers
#------------------------------------------------------------------------------------------
SET(CORE_VEHICLE_TEST_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_VEHICLE_TEST_INC_PATH ${CORE_VEHICLE_TEST_PATHS} CACHE STRING
        "Path(s) to the core vehicle test include files" FORCE)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME VehicleTest)

#------------------------------------------------------------------------------------------
# Add the ServicesLayer include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libvehicletest.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_VEHICLE_TEST_LIB_PATH})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} vehicletest)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})
