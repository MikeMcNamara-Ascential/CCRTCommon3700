#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME IsuzuEmissionsTC)
#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(PIMEXE_NAME IsuzuPIMTC)

#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell the compiler where to find the module interface files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
		    ${CORE_ECU_TC_INC_PATH} 
		    ${CORE_PROTOCOLS_INC_PATH}
		    ${ISUZU_ENGINE_MODULE_INC_PATH}
		    )


LINK_DIRECTORIES(${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH} ${CORE_ECU_TC_LIB_PATH})

LINK_LIBRARIES(moduleinterfaces protocolfilters emissioncomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)
#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${PIMEXE_NAME} ${PIMEXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${PIMEXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
