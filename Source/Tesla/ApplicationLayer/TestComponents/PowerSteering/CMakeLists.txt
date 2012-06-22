#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE COMPONENT_SRC *.cpp)

#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME TeslaPowerSteeringTC)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(    ${CUSTOMER_EPAS_MODULE_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CUSTOMER_COMMON_TC_LIB_PATH} 
		 ${CUSTOMER_MODULES_LIB_PATH} 
		 ${CUSTOMER_PROTOCOLS_LIB_PATH}
		 ${CORE_COMMON_TC_LIB_PATH} 
		 ${CORE_MODULES_LIB_PATH} 
		 ${CORE_PROTOCOLS_LIB_PATH})
LINK_LIBRARIES(commoncomponent protocolfilters)
#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${COMPONENT_SRC})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

