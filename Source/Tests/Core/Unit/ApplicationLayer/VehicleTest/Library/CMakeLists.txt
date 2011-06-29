#-------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#-------------------------------------------------------------------------------
SET(EXE_NAME VehicleTestLibraryTest)

#-------------------------------------------------------------------------------
# Where to find the Necessary .h files
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_VEHICLE_TEST_INC_PATH})

#-------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#-------------------------------------------------------------------------------
ADD_LIBRARY(vehicletestlibrarytestsuite 
                ${EXE_NAME}Suite.cpp 
                ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(vehicletestlibrarytestsuite
                          vehicletest)
                          
#-------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#-------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner vehicletestlibrarytestsuite)
