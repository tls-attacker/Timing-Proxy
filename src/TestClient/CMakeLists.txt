project(TestClient)

set(CMAKE_CXX_STANDARD 17)
add_executable(TestClient main.cpp ../helper/helper.h $<TARGET_OBJECTS:TimingSocket> $<TARGET_OBJECTS:TimeSources>)
