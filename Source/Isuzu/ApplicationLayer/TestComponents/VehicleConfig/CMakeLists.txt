#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME MimamoriTc)

#------------------------------------------------------------------------------------------
# Tell the compiler where to find the module interface files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
		    ${CORE_ECU_TC_INC_PATH} 
		    ${CORE_PROTOCOLS_INC_PATH} 
                    ${CUSTOMER_PROTOCOLS_INC_PATH} 
		    ${MIMAMORI_MODULE_INC_PATH})


LINK_DIRECTORIES(${CORE_MODULES_LIB_PATH} 
		 ${CORE_PROTOCOLS_LIB_PATH} 
		 ${CORE_ECU_TC_LIB_PATH}
		 ${CUSTOMER_PROTOCOL_PATH})

LINK_LIBRARIES(moduleinterfaces protocolfilters isuzuprotocolfilter)

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
