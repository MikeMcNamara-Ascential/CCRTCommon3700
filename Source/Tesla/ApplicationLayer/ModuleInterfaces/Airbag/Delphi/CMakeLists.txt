#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Delphi 
#------------------------------------------------------------------------------------------
SET( CUSTOMER_DELPHI_AIRBAG_MODULE_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH 
	"Path to the customer Delphi Airbag Module Interface" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

