#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonEmissionsTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_COMMON_TC_INC_PATH} 
                    ${CORE_ECU_TC_INC_PATH}
                    ${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/ProtocolFilter
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/TestResultServer
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/GenericEmissionsModuleTemplate)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commonemissionstestsuite 
                ${EXE_NAME}Suite.cpp 
                ${EXE_NAME}.cpp)
TARGET_LINK_LIBRARIES(commonemissionstestsuite 
                          commoncomponent
                          moduleinterfaces
                          protocolfilters
                          mockprotocolfilterlib
                          mocktestresultserverlib
                          mocknameddatabrokerlib
                          mockgenericemissionsmoduletemplatelib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commonemissionstestsuite)
