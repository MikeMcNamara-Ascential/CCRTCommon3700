#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Transmission FILES
	ManualTransmissionConfig.xml
	ManualTransmissionConfigSPDL.xml
	TransmissionFaultList.xml)
