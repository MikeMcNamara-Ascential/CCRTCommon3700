#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES
	      Tesla.SerialServerConfig.xml
		  Tesla.Freemont.507560.SerialServerConfig.xml
                  Tesla.Freemont.508180.SerialServerConfig.xml
                  Tesla.Freemont.508202.SerialServerConfig.xml
                  Tesla.Freemont.508203.SerialServerConfig.xml
		  Tesla.MenloPark.507211.SerialServerConfig.xml
	      )
