#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Isuzu Allison 
#------------------------------------------------------------------------------------------
SET( ISUZU_ALLISON_MODULE_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH 
	"Path to the Isuzu ALLISON Module Interface" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
