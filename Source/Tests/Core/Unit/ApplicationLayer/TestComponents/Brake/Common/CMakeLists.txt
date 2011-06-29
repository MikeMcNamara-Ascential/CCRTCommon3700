#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME CommonBrakeTest)

#------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_BRAKE_TC_INC_PATH} 
                    ${CORE_COMMON_TC_INC_PATH}
                    ${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/ProtocolFilter
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/GenericTc)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------
ADD_LIBRARY(commonbraketestsuite 
                ${EXE_NAME}Suite.cpp 
                BaseBrakeToolTest.cpp 
                GenericABSTCTemplateTest.cpp)
                
TARGET_LINK_LIBRARIES(commonbraketestsuite 
                          basebrake 
                          commoncomponent
                          mockprotocolfilterlib
                          mockgenerictclib)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commonbraketestsuite)
