#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/LossCompensation FILES 
    GM.LossCompensation.xml
    GM.AMG.LossCompensation.xml
    GM.Holden.LossCompensation.xml
    GM.Gravatai.LossCompensation.xml
    GM.Gunsan.LossCompensation.xml
    GM.SaoCaetano.3658.LossCompensation.xml
    GM.SaoJose.3958.LossCompensation.xml
    GM.SaoJose.LossCompensation.xml
)
