#------------------------------------------------------------------------------------------
# Add the Motor include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Motor)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Motor)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libmotorcontroller.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Motor)
LINK_LIBRARIES( motor)

ADD_SUBDIRECTORY(Baldor)
ADD_SUBDIRECTORY(AB)

