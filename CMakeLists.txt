cmake_minimum_required(VERSION 3.10)


project(networkTimingTool)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD 17)

subdirs(src)
subdirs(tests)