project(TestServer)

set(CMAKE_CXX_STANDARD 17)
add_executable(TestServer main.cpp ../helper/helper.h $<TARGET_OBJECTS:Socket> $<TARGET_OBJECTS:ServerSocket>)
