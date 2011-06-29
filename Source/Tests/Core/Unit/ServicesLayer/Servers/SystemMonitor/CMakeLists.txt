#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME SystemMonitorTest)

#------------------------------------------------------------------------------------------
# Where to find the .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/SystemMonitor/Include
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/SystemMonitor
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/PromptServer
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/FaultServer)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(systemmonitortestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(systemmonitortestsuite 
                          sysmon
                          mocksystemmonitorlib
                          mocknameddatabrokerlib
                          mockpromptserverlib
                          mockfaultserverlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner systemmonitortestsuite)

