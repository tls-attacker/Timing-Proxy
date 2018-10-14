PROJECT(PacketParserTests)

add_executable(PacketParserTests ../PacketParser.cpp ../PacketParser.h ../Ethernet/EthernetParser.cpp ../Ethernet/EthernetParser.h ../Ipv4/Ipv4Parser.h ../Ipv4/Ipv4Parser.cpp ../Ipv6/Ipv6Parser.h ../Ipv6/Ipv6Parser.cpp ../Tcp/TcpParser.h ../Tcp/TcpParser.cpp PacketParserTests.h PacketParserTests.cpp)
