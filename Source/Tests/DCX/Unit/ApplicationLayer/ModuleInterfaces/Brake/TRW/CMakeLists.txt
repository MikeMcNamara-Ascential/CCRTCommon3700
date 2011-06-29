#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME TrwBrakeModuleInterfacesTest)

#------------------------------------------------------------------------------------------
# Where to find the Necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH}
                    ${DCX_MODULES_INC_PATH}
		    ${DCX_PROTOCOLS_INC_PATH}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/ProtocolFilter
                    ${BRAKE_MODULE_PATHS})

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(trwbrakemoduleinterfacestestsuite 
                ${EXE_NAME}Suite.cpp 
                GenericTrwABSModuleTemplateTest.cpp)
                
TARGET_LINK_LIBRARIES(trwbrakemoduleinterfacestestsuite
                          moduleinterfaces
                          dcxmoduleinterfaces)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner trwbrakemoduleinterfacestestsuite)
