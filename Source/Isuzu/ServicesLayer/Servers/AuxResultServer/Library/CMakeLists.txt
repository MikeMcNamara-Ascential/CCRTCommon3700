#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_LIBRARY(AuxResultServer STATIC 
			AuxResultServer.cpp
			AuxResultReceiverThread.cpp
			AuxResultFieldList.cpp)

