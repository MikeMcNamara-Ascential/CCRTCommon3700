#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME LossCompensationTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_COMMON_TC_INC_PATH} 
                    ${CORE_LC_TC_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/FaultServer
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(losscompensationtestsuite 
                ${EXE_NAME}Suite.cpp 
                ${EXE_NAME}.cpp)
                
TARGET_LINK_LIBRARIES(losscompensationtestsuite 
                          losscompensationcomponent
                          mockfaultserverlib
                          mocknameddatabrokerlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner losscompensationtestsuite)
