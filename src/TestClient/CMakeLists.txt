project(TestClient)

set(CMAKE_CXX_STANDARD 17)
add_executable(TestClient main.cpp ../helper/helper.h $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:PcapWrapper> $<TARGET_OBJECTS:TimeSources> $<TARGET_OBJECTS:PacketParser>)
find_package(PCAP REQUIRED)
include_directories(${PCAP_INCLUDE_DIR})
target_link_libraries(TestClient ${PCAP_LIBRARY})

FIND_PACKAGE ( Threads REQUIRED )
TARGET_LINK_LIBRARIES (TestClient ${CMAKE_THREAD_LIBS_INIT} )