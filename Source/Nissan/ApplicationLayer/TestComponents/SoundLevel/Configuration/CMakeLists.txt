#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/SoundLevel FILES
        DSP80Protocol.xml
        HY114Protocol.xml
        SoundLevelComponent.xml
        SoundLevelComponentHY114.xml
        Nissan.Changzhou.SoundLevelComponentHY114.xml
	SoundLevelFaultList.xml)
