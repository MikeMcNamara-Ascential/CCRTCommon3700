#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonMotorControllerTest)

#------------------------------------------------------------------------------------------
# Where to find the PromptServer .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commonmotorcontrollertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    CommonMotorControllerTest.cpp)

TARGET_LINK_LIBRARIES(commonmotorcontrollertestsuite motorcontroller)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commonmotorcontrollertestsuite)

