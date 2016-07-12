#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Library)
LINK_LIBRARIES(AuxResultServer)

#------------------------------------------------------------------------------------------
# We need to include stuff from the parser library directory
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)
#ADD_SUBDIRECTORY(HLAResultServer)
ADD_SUBDIRECTORY(SideSlipResultServer)

