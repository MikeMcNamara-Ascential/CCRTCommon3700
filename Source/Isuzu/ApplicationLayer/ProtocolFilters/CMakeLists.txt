SET(CUSTOMER_PROTOCOLS_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the root of the customer protocol filter tree" FORCE)

SET(CUSTOMER_PROTOCOL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Isuzu)
SET(CUSTOMER_PROTOCOL_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE STRING
	"Path to customer protocol filters" FORCE)

INCLUDE_DIRECTORIES(${CORE_PROTOCOLS_INC_PATH} 
		    ${CUSTOMER_PROTOCOL_INC_PATH})	
	 
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Isuzu)


