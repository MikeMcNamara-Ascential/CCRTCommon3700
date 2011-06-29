#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonMachineTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_COMMON_TC_INC_PATH} 
                    ${CORE_MACHINE_TC_INC_PATH}
                    ${CORE_SERVICES_LAYER_DIR}/Servers/PromptServer
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/TestResultServer
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/PromptServer)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commonmachinetestsuite 
                ${EXE_NAME}Suite.cpp 
                ${EXE_NAME}.cpp)
                
TARGET_LINK_LIBRARIES(commonmachinetestsuite 
                          machinecomponent 
                          commoncomponent
                          mocktestresultserverlib
                          mocknameddatabrokerlib
                          mockpromptserverlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commonmachinetestsuite)
