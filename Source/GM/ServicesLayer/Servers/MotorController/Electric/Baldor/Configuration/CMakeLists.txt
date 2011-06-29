#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${MACHINE_CONFIG_DIR} FILES
    GM.BaldorMotorControllerConfig.xml
    GM.AMG.BaldorMotorControllerConfig.xml
    GM.SaoCaetano.BaldorMotorControllerConfig.xml
)
