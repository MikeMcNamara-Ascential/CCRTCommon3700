#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the EPAS 
#------------------------------------------------------------------------------------------
SET( CUSTOMER_EPAS_MODULE_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH 
	"Path to the customer PowerSteering Module Interface" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

