#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(ModuleInterfaces)
ADD_SUBDIRECTORY(ProtocolFilters)
ADD_SUBDIRECTORY(TestComponents)
ADD_SUBDIRECTORY(VehicleTest)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ApplicationLayerTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(applicationtestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(applicationtestsuite 
                          moduleinterfacestestsuite
                          protocolfilterstestsuite
                          testcomptestsuite
                          vehicletesttestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner applicationtestsuite)

