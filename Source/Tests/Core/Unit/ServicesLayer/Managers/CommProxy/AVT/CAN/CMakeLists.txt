#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME AvtCanProxyTest)

#------------------------------------------------------------------------------------------
# Where to find the AvtCanProxy .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Managers/Include
                    ${CORE_SERVICES_LAYER_DIR}/Managers/CommProxy/Include
                    ${CORE_SERVICES_LAYER_DIR}/Managers/CommProxy/AVT/CAN
                    ${CORE_SERVICES_LAYER_DIR}/Managers/CommProxy/AVT/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(avtcanproxytestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(avtcanproxytestsuite avtcanproxylib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner avtcanproxytestsuite)

