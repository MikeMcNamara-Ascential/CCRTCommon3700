#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)
ADD_SUBDIRECTORY(Managers)
ADD_SUBDIRECTORY(Servers)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ServicesLayerTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(serviceslayertestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(serviceslayertestsuite librarytestsuite
                                             managerstestsuite
                                             serverstestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner serviceslayertestsuite)

