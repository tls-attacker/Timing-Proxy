#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
#include <ctime>
//#include <Kernel/i386/cpuid.h>
#include "Socket/ClientSocket/TimingSocket/TimingSocket.h"
#include "TimingProxy/TimingProxy.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;


int main(int argc, const char** argv) {
    uint16_t dataPort, control_port;
    Socket::TimingSocket::KindOfSocket measurement_technique;
    std::string interface;
    std::string technique;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("dataport,d", po::value< uint16_t >(&dataPort)->default_value(4444), "set listen port for proxy data")
            ("controlport,c", po::value< uint16_t >(&control_port)->default_value(5555), "set listen port for proxy control")
            ("technique,t", po::value<std::string>(&technique), "set timing measurement technique to one of CPU, Kernel, PCAP")
            ("interface,i", po::value<std::string>(&interface)->default_value("lo"), "set interface to be used by PCAP");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (technique == "CPU") {
        measurement_technique = Socket::TimingSocket::KindOfSocket::CPU;
    }else if(technique == "Kernel"){
        measurement_technique = Socket::TimingSocket::KindOfSocket::Kernel;
    }else if(technique == "PCAP") {
        measurement_technique = Socket::TimingSocket::KindOfSocket::PCAP;
    }else{
        std::cout << "Invalid measurement technique. See \"-h\" for help." << std::endl;
        return -1;
    }

    TimingProxy timingProxy(dataPort, control_port, measurement_technique);
    timingProxy.setInterface(interface);
    timingProxy.run();
    return 0;
}
