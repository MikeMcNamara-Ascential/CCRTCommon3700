#------------------------------------------------------------------------------------------
# All components need the protocol filter and module interface headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CUSTOMER_MODULES_INC_PATH}
                    ${CUSTOMER_PROTOCOLS_INC_PATH}
                    ${CORE_COMMON_TC_INC_PATH}
                    ${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the customer module interfaces library
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(commoncomponent
               moduleinterfaces
               protocolfilters)

#------------------------------------------------------------------------------------------
# All components link against the core protocol filter library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CUSTOMER_MODULES_LIB_PATH} ${CUSTOMER_PROTOCOLS_LIB_PATH}
                 ${CORE_COMMON_TC_LIB_PATH} ${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(Machine)
ADD_SUBDIRECTORY(Transmission)
