#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME WinCcrtInterface)
SET(EXE_NAME_2 WinPendantInterface)

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
ADD_LIBRARY(${LIB_NAME} STATIC WinCcrtInterface.cpp )

STRING(TOLOWER ${EXE_NAME_2} TMP_LIB_NAME_2)
SET(LIB_NAME_2 ${TMP_LIB_NAME_2}lib)
ADD_LIBRARY(${LIB_NAME_2} STATIC WinPendantInterface.cpp )

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME} -lds -lm)

CREATE_QXE(${EXE_NAME_2} ${EXE_NAME_2}Main.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME_2} ${LIB_NAME_2} -lds -lm)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
#ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)
#ADD_USE_MSSG(${EXE_NAME_2} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME_2}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME_2})

