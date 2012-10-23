#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the root of the module interface tree
#------------------------------------------------------------------------------------------
SET(CUSTOMER_MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the root of the customer module interfaces tree" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the customermoduleinterfaces.a library
#------------------------------------------------------------------------------------------
SET(CUSTOMER_MODULES_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the customer module interfaces library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the module interface headers
#------------------------------------------------------------------------------------------
SET(CUSTOMER_MODULE_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Include
                ${CMAKE_CURRENT_SOURCE_DIR}/Common)
SET(CUSTOMER_MODULES_INC_PATH ${CUSTOMER_MODULE_PATHS} CACHE STRING
    "Path to the customer module interfaces include files" FORCE)

#------------------------------------------------------------------------------------------
# Where are our headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
		    ${CUSTOMER_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH} 
		    ${CUSTOMER_PROTOCOLS_INC_PATH})

#------------------------------------------------------------------------------------------
# Where is the core module interface library located
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_MODULES_DIR}/Common)

#------------------------------------------------------------------------------------------
# Build the individual module interfaces next
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Airbag)
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(Gateway)
ADD_SUBDIRECTORY(PowerSteering)
