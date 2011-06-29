#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME SystemScreenSaverTest)

#------------------------------------------------------------------------------
# Where to find the needed header files
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/SystemScreenSaver)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------
ADD_LIBRARY(systemscreensavertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(systemscreensavertestsuite systemscreensaverlib)

#------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner systemscreensavertestsuite)

