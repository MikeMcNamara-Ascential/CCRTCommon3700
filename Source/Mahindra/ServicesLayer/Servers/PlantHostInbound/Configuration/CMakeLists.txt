#------------------------------------------------------------------------------------------
# Build a list of all the XML files
#------------------------------------------------------------------------------------------
FILE(GLOB PHI_CONFIG_FILES *.xml)

#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES ${PHI_CONFIG_FILES})
