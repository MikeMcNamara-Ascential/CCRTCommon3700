#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES
    GM.PlcConfig.xml 
    GM.AMG.SerialPlcConfig.xml
    GM.Gunsan.SerialPlcConfig.xml
    GM.Holden.SerialPlcConfig.xml
    GM.SaoCaetano.3658.SerialPlcConfig.xml
)

