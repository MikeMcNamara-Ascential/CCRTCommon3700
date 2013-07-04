#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME MazdaMapleKeypadMgr.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------
# Add the InputServer Include directory to the list of include directories
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVERS_DIR}/InputServer/Include)

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} MazdaMapleKeypadMgrMain.cpp 
                           MazdaMapleKeypadMgr.cpp)
                           
#------------------------------------------------------------------------------
# Link to the core symbol scanner library
#------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${EXE_NAME} InputDevicelib maplekeypadmanagerlib)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
