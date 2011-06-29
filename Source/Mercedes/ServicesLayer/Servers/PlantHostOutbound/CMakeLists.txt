#------------------------------------------------------------------------------------------
# Tell the linker where the core PlantHostOutbound libraries are
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_HOST_INTERFACE_LIB_DIR})

#------------------------------------------------------------------------------------------
# Add the Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_PLANT_HOST_OUT_INC_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(PFS)
ADD_SUBDIRECTORY(Configuration)
