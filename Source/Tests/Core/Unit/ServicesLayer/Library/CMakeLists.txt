#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(XmlProcessor)
ADD_SUBDIRECTORY(SystemCommunication)
ADD_SUBDIRECTORY(Debuggable)
ADD_SUBDIRECTORY(SoftwareId)
ADD_SUBDIRECTORY(NotificationThreadSpinner)
ADD_SUBDIRECTORY(GeneralUtilities)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME LibraryTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(librarytestsuite STATIC ${EXE_NAME}Suite.cpp BepServerTest.cpp)
TARGET_LINK_LIBRARIES(librarytestsuite
    debuggabletestsuite
    softwareidtestsuite
    syscommtestsuite
    xmlparsertestsuite
    notificationthreadspinnertestsuite
    generalutilitiestestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner librarytestsuite )

