#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Tachograph FILES 
	      SwitchSettingTable.dat
	      TachographFaults.xml
	      TachographTC.xml
	      )
