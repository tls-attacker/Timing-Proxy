project(TestClient)

set(CMAKE_CXX_STANDARD 17)
add_executable(TestClientPasive main.cpp ../helper/helper.h $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:TimeSources> $<TARGET_OBJECTS:PcapWrapper> $<TARGET_OBJECTS:PacketParser>)

find_package(PCAP REQUIRED)
include_directories(${PCAP_INCLUDE_DIR})
target_link_libraries(TestClientPasive ${PCAP_LIBRARY})

FIND_PACKAGE ( Threads REQUIRED )
TARGET_LINK_LIBRARIES (TestClientPasive ${CMAKE_THREAD_LIBS_INIT} )