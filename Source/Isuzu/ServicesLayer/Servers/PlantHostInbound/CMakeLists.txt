#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME IsuzuPlantHostInbound.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------
# Add the PlantHostInbound include directory to the list of include directories
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_PLANT_HOST_IN_INC_DIR})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------
# Make sure the linker can find libplanthostinbound.a once it is built.
#------------------------------------------------------------------------------
LINK_DIRECTORIES(CORE_PLANT_HOST_IN_LIB_DIR})

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} IsuzuPlantHostInboundMain.cpp 
                           IsuzuPlantHostInbound.cpp)
                           
#------------------------------------------------------------------------------
# All PlantHostInbound processes link against libplanthostinbound.a
#------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${EXE_NAME} PlantHostIn)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
