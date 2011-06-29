#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME GeneralUtilitiesTest)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------
ADD_LIBRARY(generalutilitiestestsuite STATIC ${EXE_NAME}Suite.cpp ${EXE_NAME}.cpp)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner generalutilitiestestsuite)

