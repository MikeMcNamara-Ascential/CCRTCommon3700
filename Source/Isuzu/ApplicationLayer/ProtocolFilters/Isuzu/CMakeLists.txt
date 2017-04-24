INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Include)
SET(ISUZU_PROTOCOL_SOURCES  IsuzuCommonInfoProtocolFilter.cpp FordCanProtocolFilter.cpp)
ADD_LIBRARY(isuzuprotocolfilter STATIC ${ISUZU_PROTOCOL_SOURCES})
