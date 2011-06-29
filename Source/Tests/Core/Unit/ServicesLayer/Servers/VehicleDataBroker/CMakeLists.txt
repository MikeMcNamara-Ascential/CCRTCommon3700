#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME VehicleDataBrokerTest)

#------------------------------------------------------------------------------------------
# Where to find the VehicleDataBroker .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_VEHICLE_DATA_BROKER_INC_DIR}
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/NamedDataBroker)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(vehicledatabrokertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(vehicledatabrokertestsuite 
                          mocknameddatabrokerlib
                          vehicledatabrokerlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner vehicledatabrokertestsuite)

