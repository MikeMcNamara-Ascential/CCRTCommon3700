#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Bosch8 
#------------------------------------------------------------------------------------------
SET( BOSCH8_MODULE_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH 
	"Path to the Bosch 8 Module Interface" FORCE)

ADD_SUBDIRECTORY(Configuration)

