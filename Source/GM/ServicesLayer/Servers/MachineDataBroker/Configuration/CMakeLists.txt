#------------------------------------------------------------------------------------------
# Tell CMake where to install the config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${CUSTOMER_SERVICES_CFG_DEPLOY_DIR} FILES 
    GM.MachineDataBrokerConfig.xml
    GM.MachineDataBroker360Config.xml
    GM.MachineDataBroker3600Config.xml
    GM.AMG.MachineDataBrokerConfig.xml
    GM.Gunsan.MachineDataBrokerConfig.xml
    GM.Holden.MachineDataBrokerConfig.xml
    GM.SaoCaetano.3658.MachineDataBrokerConfig.xml
)
