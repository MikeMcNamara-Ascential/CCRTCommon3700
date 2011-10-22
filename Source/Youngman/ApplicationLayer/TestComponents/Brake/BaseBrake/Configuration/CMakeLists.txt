#------------------------------------------------------------------------------------------
# Tell CMake what files to install
#------------------------------------------------------------------------------------------
FILE(GLOB installFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES
        ${installFiles})
