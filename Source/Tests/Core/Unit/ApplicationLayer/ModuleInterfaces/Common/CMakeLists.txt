#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonModuleInterfacesTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
                    ${CORE_PROTOCOLS_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/ProtocolFilter)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commonmoduleinterfacestestsuite 
                ${EXE_NAME}Suite.cpp 
                BackgroundMonitorTest.cpp
                MockBackgroundMonitor.cpp
                ReplyInterpreterTest.cpp
                SendModuleKeepAliveTest.cpp
                GenericModuleTemplateTest.cpp)
                
TARGET_LINK_LIBRARIES(commonmoduleinterfacestestsuite
                          moduleinterfaces
                          protocolfilters
                          mockprotocolfilterlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commonmoduleinterfacestestsuite)
