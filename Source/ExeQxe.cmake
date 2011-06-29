#===============================================================================================
# Make sure executables have the correct extension by creating a symbolic File.qxe link that
# points to File.exe
#===============================================================================================
IF(NOT CMAKE_SYSTEM MATCHES "QNX*")
	ADD_CUSTOM_TARGET(${EXE_NAME}.qxe
			ALL
			ln -sf ${EXE_NAME}.exe ${EXE_NAME}.qxe
			DEPENDS ${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX})
ENDIF(NOT CMAKE_SYSTEM MATCHES "QNX*")

