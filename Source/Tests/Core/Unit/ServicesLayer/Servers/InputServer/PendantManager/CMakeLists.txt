#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME PendantManagerTest)

#------------------------------------------------------------------------------------------
# Where to find the PendantManager .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/InputServer/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(pendantmanagertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(pendantmanagertestsuite pendantmanagerlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner pendantmanagertestsuite)

