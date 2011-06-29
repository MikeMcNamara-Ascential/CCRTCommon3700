##===============================================================================================
## Add a QNX use message to the executable
##
## What we are doing here is:
## 1. Create a pseudo-target for adding a use message to the executable
## 2. Adding a QNX "use" message to the executable
##===============================================================================================
#
## Make sure a source file for a use message is set
#IF(USE_FILE_NAME)
#	# Look for 'ProcessName.qxe'
#	GET_TARGET_PROPERTY(QXE_PATH ${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX} LOCATION)
#	IF(QXE_PATH)
#
#		ADD_CUSTOM_COMMAND(TARGET ${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX}
#				POST_BUILD
#				COMMAND ${USE_CMD}
#				ARGS -c ${QXE_PATH} ${USE_FILE_NAME}
#				COMMENT Add a usage message to an executable)
#
#	ELSE(QXE_PATH)
#
#		# 'ProcessName.qxe' is not the target name, so look for simply 'ProcessName'
#		GET_TARGET_PROPERTY(EXE_PATH ${EXE_NAME} LOCATION)
#		IF(EXE_PATH)
#
#			ADD_CUSTOM_COMMAND(TARGET ${EXE_NAME}
#					POST_BUILD
#					COMMAND ${USE_CMD}
#					ARGS -c ${EXE_PATH} ${USE_FILE_NAME}
#					COMMENT Add a usage message to an executable)
#		ENDIF(EXE_PATH)
#
#	ENDIF(QXE_PATH)
#
#ENDIF(USE_FILE_NAME)
