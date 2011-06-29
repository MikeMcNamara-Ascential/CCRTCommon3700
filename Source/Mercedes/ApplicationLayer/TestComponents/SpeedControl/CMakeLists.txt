#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME MercedesSpeedControlTc)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( ${CORE_MODULES_INC_PATH} 
		     ${CORE_SPEEDCTRL_TC_INC_PATH} 
		     ${CORE_PROTOCOLS_INC_PATH}
		     ${CORE_COMMON_TC_INC_PATH}
		     )

LINK_DIRECTORIES(${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH} ${CORE_COMMON_TC_LIB_PATH})
LINK_LIBRARIES(moduleinterfaces protocolfilters commoncomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
