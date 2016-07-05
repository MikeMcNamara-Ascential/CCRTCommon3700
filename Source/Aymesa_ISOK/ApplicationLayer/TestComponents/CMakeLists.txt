#------------------------------------------------------------------------------------------
# All components need the protocol filter and module interface headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} ${CORE_PROTOCOLS_INC_PATH} ${CORE_COMMON_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH} ${CORE_COMMON_TC_LIB_PATH})
LINK_LIBRARIES( moduleinterfaces protocolfilters commoncomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(Machine)
ADD_SUBDIRECTORY(TransferCase)
