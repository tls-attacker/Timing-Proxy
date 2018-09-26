add_subdirectory(pcapMeasure)
add_subdirectory(TimeSources)
add_subdirectory(TimingSocket)
add_subdirectory(Benchmark)

add_executable(networkTimingTool main.cpp $<TARGET_OBJECTS:pcapMeasure> $<TARGET_OBJECTS:TimeSources> $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:Benchmark>)

find_package(PCAP)
include_directories(${PCAP_INCLUDE_DIR})
target_link_libraries(networkTimingTool ${PCAP_LIBRARY})