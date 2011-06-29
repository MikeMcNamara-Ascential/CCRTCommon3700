#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
# Server config files
	GM.AMG.Gen3InterfaceConfig.xml
	GM.Holden.Gen3InterfaceConfig.xml
	Gen3InterfaceConfig.xml
# Wheelbase config files
	GM.AMG.WheelbasePositionTable.xml
	GM.Dongyue.WheelbasePositionTable.xml
	GM.Gravatai.WheelbasePositionTable.xml
	GM.Gunsan.WheelbasePositionTable.xml
	GM.Halol.WheelbasePositionTable.xml
	GM.Holden.WheelbasePositionTable.xml
	GM.Ramos.WheelbasePositionTable.xml
	GM.Rosario.WheelbasePositionTable.xml
	GM.SaoCaetano.WheelbasePositionTable.xml
	GM.SaoCaetano.507400.WheelbasePositionTable.xml
	GM.Thailand.WheelbasePositionTable.xml
	GM.Thailand.507392.WheelbasePositionTable.xml
	GM.Uzbekistan.WheelbasePositionTable.xml
	GM.Vietnam.WheelbasePositionTable.xml
	WheelbasePositionTable.xml
	)

