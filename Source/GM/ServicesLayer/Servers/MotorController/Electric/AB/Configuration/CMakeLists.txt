#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${MACHINE_CONFIG_DIR} FILES
	GM.ABMotorControllerConfig.xml
	GM.AMG.ABMotorControllerConfig.xml
	GM.Holden.ABMotorControllerConfig.xml
	GM.Ramos.ABMotorControllerConfig.xml
)
