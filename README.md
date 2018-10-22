# Network Timing Tool

## Dependencies
This project uses C++17. You should have a recent version of gcc / clang installed
Also, cmake 3.10 or newer is needed for compilation
- libpcap

This is how you could install all dependencies and compile on debian
```bash
sudo apt install git clang libpcap-dev
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make
```

The main binary will be placed in the `bin/src` folder.
