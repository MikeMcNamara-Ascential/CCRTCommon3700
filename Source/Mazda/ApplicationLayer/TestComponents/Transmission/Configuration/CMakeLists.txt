#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Transmission FILES
	MazdaManualTransmissionConfig.xml
	MazdaAutoTransmissionConfig.xml
	TransmissionFaultList.xml)
