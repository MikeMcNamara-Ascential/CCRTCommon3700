#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ElectricMotorTest)

#------------------------------------------------------------------------------------------
# Where to find the PromptServer .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Include
                    ${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Electric/Motor)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(electricmotortestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ElectricMotorTest.cpp)

TARGET_LINK_LIBRARIES(electricmotortestsuite motor)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner electricmotortestsuite)

