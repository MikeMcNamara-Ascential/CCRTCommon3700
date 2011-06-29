#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
					${DCX_MODULES_INC_PATH}
					${DCX_PROTOCOLS_INC_PATH}
					${CORE_COMMON_TC_INC_PATH}
					${CORE_PROTOCOLS_INC_PATH}
					${CORE_MODULES_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the protocol filter, module interface and test component 
# libraries
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${DCX_MODULES_LIB_PATH} ${DCX_PROTOCOLS_LIB_PATH}
				${CORE_COMMON_TC_LIB_PATH} ${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH})
LINK_LIBRARIES(commoncomponent dcxprotocolfilters dcxmoduleinterfaces)

#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME XMLStressTest)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}.cpp)
