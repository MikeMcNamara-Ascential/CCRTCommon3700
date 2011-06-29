#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME HydraulicMotorControllerTest)

#------------------------------------------------------------------------------------------
# Where to find the necessary header files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Hydraulic
                    ${CORE_SERVICES_LAYER_DIR}/Servers/MotorController/Include
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(hydraulicmotorcontrollertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    HydraulicMotorControllerTest.cpp)

TARGET_LINK_LIBRARIES(hydraulicmotorcontrollertestsuite 
                          hydraulicmotorcontrollerlib
                          mocknameddatabrokerlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner hydraulicmotorcontrollertestsuite)

