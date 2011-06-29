#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES 
	      DCX.DCBR.BrakeFaultList.xml 
	      KwpKnorrTC.xml
	      )
