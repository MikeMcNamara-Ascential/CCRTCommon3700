#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES($ENV{QNX_TARGET}/usr/local/lib)
LINK_LIBRARIES(cppunit)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
#ADD_SUBDIRECTORY(MockClasses)
#ADD_SUBDIRECTORY(ServicesLayer)
ADD_SUBDIRECTORY(ApplicationLayer)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME DCXUnitTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(dcxunittestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(dcxunittestsuite dcxapplicationtestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner dcxunittestsuite)

