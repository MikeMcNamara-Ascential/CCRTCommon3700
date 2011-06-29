#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Emissions)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(Machine)
ADD_SUBDIRECTORY(SpeedControl)
ADD_SUBDIRECTORY(Transmission)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME TestCompTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(testcomptestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(testcomptestsuite 
                          braketestsuite 
                          commontestcomponentstestsuite
                          emissionstestsuite
                          losscompensationtestsuite
                          machinetestsuite
                          speedcontroltestsuite
                          transmissiontestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner testcomptestsuite)

