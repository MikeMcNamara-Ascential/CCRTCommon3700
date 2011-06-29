#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Electric)
ADD_SUBDIRECTORY(Hydraulic)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME MotorControllerTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(motorcontrollertestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(motorcontrollertestsuite
        commonmotorcontrollertestsuite
        electricmotorcontrollertestsuite
        hydraulicmotorcontrollertestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner motorcontrollertestsuite)

