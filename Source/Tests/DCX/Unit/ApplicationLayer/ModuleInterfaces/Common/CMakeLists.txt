#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonDCXModuleInterfacesTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
                    ${CORE_PROTOCOLS_INC_PATH}
                    ${DCX_MODULES_INC_PATH}
		    ${DCX_PROTOCOLS_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/ProtocolFilter
                    ${CCRT_ROOT}/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commondcxmoduleinterfacestestsuite 
                ${EXE_NAME}Suite.cpp 
                GenericDCXCANModuleTest.cpp
                GenericDCXSCIModuleTest.cpp)
                
TARGET_LINK_LIBRARIES(commondcxmoduleinterfacestestsuite
                          moduleinterfaces
                          dcxmoduleinterfaces)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commondcxmoduleinterfacestestsuite)
