#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ABMotorControllerTest)

#------------------------------------------------------------------------------------------
# Where to find the PromptServer .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Include
                    ${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Electric/Motor
                    ${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Electric/AB)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(abmotorcontrollertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    AB700sMotorTest.cpp
    ABMotorControllerTest.cpp)

TARGET_LINK_LIBRARIES(abmotorcontrollertestsuite abmotorcontrollerlib motor)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner abmotorcontrollertestsuite)

