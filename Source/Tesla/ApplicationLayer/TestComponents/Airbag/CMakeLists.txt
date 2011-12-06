#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
#LINK_DIRECTORIES(${CUSTOMER_COMMON_TC_LIB_PATH} 
#		 ${CUSTOMER_MODULES_LIB_PATH} 
#		 ${CUSTOMER_PROTOCOLS_LIB_PATH}
#		 ${CORE_COMMON_TC_LIB_PATH} 
#		 ${CORE_MODULES_LIB_PATH} 
#		 ${CORE_PROTOCOLS_LIB_PATH})
#LINK_LIBRARIES(commoncomponent protocolfilters)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Delphi)
ADD_SUBDIRECTORY(Siemens)
