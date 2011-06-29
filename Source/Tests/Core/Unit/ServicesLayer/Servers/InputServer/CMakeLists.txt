#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CLV295ScannerMgr)
ADD_SUBDIRECTORY(MapleKeypadManager)
ADD_SUBDIRECTORY(PendantManager)
ADD_SUBDIRECTORY(SymbolScannerMgr)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME InputServerTest)

#------------------------------------------------------------------------------------------
# Where to find the InputServer .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/InputServer/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(inputservertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(inputservertestsuite 
                      inputserverlib
                      clv295scannermgrtestsuite
                      maplekeypadmanagertestsuite
                      pendantmanagertestsuite
                      symbolscannermgrtestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner inputservertestsuite)

