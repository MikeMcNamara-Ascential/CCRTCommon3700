#------------------------------------------------------------------------------------------
# Add the ResManager include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Common)

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libtaskmon.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Common)
LINK_LIBRARIES(taskmonlib)

#------------------------------------------------------------------------------------------
# Build the executables after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(TaskMon)
ADD_SUBDIRECTORY(TaskMonTerm)
ADD_SUBDIRECTORY(Configuration)
ADD_SUBDIRECTORY(TaskMonOld)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the scripts
#------------------------------------------------------------------------------------------
INSTALL_PROGRAMS(${CORE_TOOLS_INSTALL_DIR} TaskMon.sh)
