#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME DensoAirbagTC)

#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(    ${CUSTOMER_DENSO_AIRBAG_MODULE_INC_PATH})

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

