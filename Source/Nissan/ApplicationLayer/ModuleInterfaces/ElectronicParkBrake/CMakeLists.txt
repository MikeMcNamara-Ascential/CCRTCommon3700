#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the TRW 
#------------------------------------------------------------------------------------------
SET( CUSTOMER_EPB_MODULE_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH 
	"Path to the customer Electronic Park Brake Module Interface" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
