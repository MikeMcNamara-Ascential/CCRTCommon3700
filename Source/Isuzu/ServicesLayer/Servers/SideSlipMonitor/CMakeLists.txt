#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME SideSlipMonitor)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Where to find the CoreId.h file
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Include)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC SideSlipMonitor.cpp )

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})

