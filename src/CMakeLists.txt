add_subdirectory(PcapWrapper)
add_subdirectory(TimeSources)
add_subdirectory(TimingSocket)
add_subdirectory(Benchmark)
add_subdirectory(ServerSocket)
add_subdirectory(TimingProxy)
add_subdirectory(PacketParser)

add_executable(networkTimingTool main.cpp helper/helper.h $<TARGET_OBJECTS:PcapWrapper> $<TARGET_OBJECTS:TimeSources> $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:Benchmark> $<TARGET_OBJECTS:ServerSocket> $<TARGET_OBJECTS:TimingProxy> $<TARGET_OBJECTS:PacketParser>)

find_package(PCAP)
include_directories(${PCAP_INCLUDE_DIR})
target_link_libraries(networkTimingTool ${PCAP_LIBRARY})

add_subdirectory(TestServer)
add_subdirectory(TestClient)
