#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME PlcMgrTest)

#------------------------------------------------------------------------------------------
# Where to find the PlcMgr .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Managers/PlcMgr
                    ${CORE_SERVICES_LAYER_DIR}/Managers/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(plcmgrtestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp
    PlcDf1Test.cpp
    IDf1ProtocolTest.cpp
    IRk512ProtocolTest.cpp
    PlcRk512Test.cpp
    MelsecProtocolTest.cpp)

TARGET_LINK_LIBRARIES(plcmgrtestsuite plcmgrlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner plcmgrtestsuite)

