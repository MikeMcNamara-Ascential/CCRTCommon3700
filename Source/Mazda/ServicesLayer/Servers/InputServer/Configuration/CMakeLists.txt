#------------------------------------------------------------------------------------------
# Tell CMake where to install the config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
	      Mazda.Braketest.InputServerConfig.xml
	      Mazda.Rolltest.InputServerConfig.xml
	      )
