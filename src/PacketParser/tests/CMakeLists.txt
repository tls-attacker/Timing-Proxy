PROJECT(PacketParserTests)

add_executable(PacketParserTests $<TARGET_OBJECTS:PacketParser> PacketParserTests.cpp PacketParserTests.h)
