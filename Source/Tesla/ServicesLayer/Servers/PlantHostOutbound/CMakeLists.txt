#------------------------------------------------------------------------------------------
# Tell the linker where the core PlantHostOutbound libraries are
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_PLANT_HOST_OUT_LIB_DIR})

#------------------------------------------------------------------------------------------
# Need to add core include directory
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_PLANT_HOST_OUT_INC_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(TeslaReportInterface)
ADD_SUBDIRECTORY(Configuration)
