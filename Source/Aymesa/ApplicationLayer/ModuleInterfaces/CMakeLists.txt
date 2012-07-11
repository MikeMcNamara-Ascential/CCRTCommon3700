#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the root of the module interface tree
#------------------------------------------------------------------------------------------
SET(KIA_MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the root of the DCX module interfaces tree" FORCE)

#------------------------------------------------------------------------------------------
# Where are our headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} 
                    #${KIA_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the individual module interfaces next
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
