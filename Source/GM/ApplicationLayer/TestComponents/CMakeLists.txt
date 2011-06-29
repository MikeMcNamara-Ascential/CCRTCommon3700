#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libcommoncomponent.a library
#------------------------------------------------------------------------------------------
SET(CUSTOMER_COMMON_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH 
	"Path to the core common test component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the common component headers
#------------------------------------------------------------------------------------------
SET(CUSTOMER_TC_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include")
SET(CUSTOMER_COMMON_TC_INC_PATH ${CUSTOMER_TC_INC_PATH} CACHE STRING 
	"Path(s) to the core common test component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CUSTOMER_COMMON_TC_INC_PATH}
					${CUSTOMER_MODULES_INC_PATH}
					${CUSTOMER_PROTOCOLS_INC_PATH}
					${CORE_COMMON_TC_INC_PATH}
					${CORE_PROTOCOLS_INC_PATH}
					${CORE_MODULES_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)
		
#------------------------------------------------------------------------------------------
# All components link against the protocol filter, module interface and test component 
# libraries
#------------------------------------------------------------------------------------------
#LINK_DIRECTORIES(${CUSTOMER_COMMON_TC_LIB_PATH} ${CUSTOMER_MODULES_LIB_PATH} ${CUSTOMER_PROTOCOLS_LIB_PATH}
#				${CORE_COMMON_TC_LIB_PATH} ${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH})
#LINK_LIBRARIES(commoncomponent moduleinterfaces fordprotocolfilters fordmoduleinterfaces)
LINK_DIRECTORIES(${CUSTOMER_COMMON_TC_LIB_PATH} ${CUSTOMER_MODULES_LIB_PATH} ${CUSTOMER_PROTOCOLS_LIB_PATH}
				${CORE_COMMON_TC_LIB_PATH} ${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH})
LINK_LIBRARIES(commoncomponent moduleinterfaces)


#------------------------------------------------------------------------------------------
# Build the executables after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(TransferCase)

