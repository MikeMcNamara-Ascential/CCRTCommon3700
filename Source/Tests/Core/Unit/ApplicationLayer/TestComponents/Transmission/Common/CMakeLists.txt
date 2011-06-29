#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonTransmissionTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_COMMON_TC_INC_PATH} 
                    ${CORE_TRANS_TC_INC_PATH}
                    ${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH})

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commontransmissiontestsuite 
                ${EXE_NAME}Suite.cpp 
                ${EXE_NAME}.cpp)
TARGET_LINK_LIBRARIES(commontransmissiontestsuite 
                          commoncomponent
                          moduleinterfaces
                          protocolfilters)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commontransmissiontestsuite)
