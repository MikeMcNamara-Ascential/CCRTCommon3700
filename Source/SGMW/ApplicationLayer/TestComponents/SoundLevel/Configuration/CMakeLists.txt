#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/SoundLevel FILES
        SoundLevelComponent.xml
        SoundLevelComponentHY114.xml
        SoundLevelComponentTES1352H.xml
        SoundLevelFaultList.xml
        HY114Protocol.xml
        TES1352HProtocol.xml
        DSP80Protocol.xml)
