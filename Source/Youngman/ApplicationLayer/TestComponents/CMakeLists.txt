#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
#		                        ${DELPHI74_MODULE_INC_PATH}
#					${APG3550_MODULE_INC_PATH}
					${CORE_COMMON_TC_INC_PATH}
					${CORE_PROTOCOLS_INC_PATH}
					${CORE_MODULES_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the protocol filter, module interface and test component 
# libraries
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_COMMON_TC_LIB_PATH} ${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH})
LINK_LIBRARIES(commoncomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(Machine)

