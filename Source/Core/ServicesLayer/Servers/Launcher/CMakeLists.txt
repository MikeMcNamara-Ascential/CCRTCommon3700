#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME Launcher)

#------------------------------------------------------------------------------------------
# Where to find the CCRTLicense.h file
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${LICENSE_LIB_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake to link against the licensing libraries
#------------------------------------------------------------------------------------------
FIND_LIBRARY(COMMON_LIBRARY
        NAMES license
        PATHS ${LICENSE_LIB_DIR}/Common
        DOC "Base licensing library" )

IF(NOT COMMON_LIBRARY)
        MESSAGE(FATAL_ERROR "Base licensing library not found!")
ENDIF(NOT COMMON_LIBRARY)

#------------------------------------------------------------------------------------------
# Tell CMake to link against the licensing libraries
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${LICENSE_LIB_DIR}/Common)
LINK_DIRECTORIES(${LICENSE_LIB_BIN_DIR})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME}
        ${EXE_NAME}Main.cpp
        ${EXE_NAME}.cpp
        LicenseTimer.cpp)

TARGET_LINK_LIBRARIES(${EXE_NAME} cmlicense license)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

