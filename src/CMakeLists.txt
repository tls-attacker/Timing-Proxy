add_subdirectory(pcapMeasure)
add_subdirectory(TimeSources)
add_subdirectory(TimingSocket)
add_subdirectory(Benchmark)
add_subdirectory(ServerSocket)
add_subdirectory(TimingProxy)

add_executable(networkTimingTool main.cpp $<TARGET_OBJECTS:pcapMeasure> $<TARGET_OBJECTS:TimeSources> $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:Benchmark> $<TARGET_OBJECTS:ServerSocket> $<TARGET_OBJECTS:TimingProxy>)

find_package(PCAP)
include_directories(${PCAP_INCLUDE_DIR})
target_link_libraries(networkTimingTool ${PCAP_LIBRARY})