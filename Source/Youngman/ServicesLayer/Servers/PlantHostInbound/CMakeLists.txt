#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME ChinaPlantHostInbound)

#------------------------------------------------------------------------------
# Add the PlantHostInbound include directory to the list of include directories
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
		     ${CORE_PLANT_HOST_IN_INC_DIR} 
		     ./Include
		     )

#------------------------------------------------------------------------------
# Make sure the linker can find libplanthostinbound.a once it is built.
#------------------------------------------------------------------------------
LINK_DIRECTORIES(CORE_PLANT_HOST_IN_LIB_DIR})

#------------------------------------------------------------------------------
# All PlantHostInbound processes link against libplanthostinbound.a
#------------------------------------------------------------------------------
LINK_LIBRARIES(PlantHostIn)

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp ${EXE_NAME}.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME} PlantHostIn)

#------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
