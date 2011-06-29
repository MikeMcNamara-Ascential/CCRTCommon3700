#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Teves)
ADD_SUBDIRECTORY(TRW)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME BrakeModuleInterfacesDCXTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(brakemoduleinterfacesdcxtestsuite 
                ${EXE_NAME}Suite.cpp)
                
TARGET_LINK_LIBRARIES(brakemoduleinterfacesdcxtestsuite
                          tevesbrakemoduleinterfacestestsuite
                          trwbrakemoduleinterfacestestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner brakemoduleinterfacesdcxtestsuite)
