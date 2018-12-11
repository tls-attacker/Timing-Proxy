#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "../Socket/ClientSocket/TimingSocket/TimingSocket.h"
#include "../Socket/ClientSocket/TimingSocket/PCAPTimingSocket/PCAPTimingSocket.h"
#include "../Socket/ClientSocket/TimingSocket/KernelTimingSocket/KernelTimingSocket.h"
#include "../PcapWrapper/PcapWrapper.h"
#include <boost/program_options.hpp>

using namespace std::chrono_literals;
using namespace std;
namespace po = boost::program_options;

#define BUFSIZE 1024
#define SAMPLESIZE 2


int main(int argc, char const *argv[])
{
    Socket::TimingSocket::KindOfSocket measurement_technique;
    std::string interface;
    std::string technique;
    std::string host;
    int port;
    std::string outfile;
    size_t measurements;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("technique,t", po::value<std::string>(&technique), "set timing measurement technique to one of CPU, Kernel, PCAP")
            ("interface,i", po::value<std::string>(&interface)->default_value("lo"), "set interface to be used by PCAP")
            ("host", po::value<std::string>(&host)->default_value("127.0.0.1"), "set host of oracle")
            ("port,p", po::value<int>(&port)->default_value(1337), "set port of oracle")
            ("outfile,o", po::value<std::string>(&outfile)->default_value("measurements.csv"), "set output csv for mona timing report")
            ("measurements,m", po::value<size_t>(&measurements)->default_value(1024), "set amount of measurements for each secret");
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

    char read_buf[BUFSIZE];
    char write_buf[BUFSIZE];
    memset(write_buf, 0, BUFSIZE);
    // strcpy(write_buf, "Testcase");
    std::string correct_case[SAMPLESIZE];
    std::vector<uint64_t> times[SAMPLESIZE];
    for (size_t i = 0; i< SAMPLESIZE; i++) {
        times[i].reserve(measurements);
    }
    uint64_t medians[SAMPLESIZE];
    uint64_t overall_median = 0;
    std::unique_ptr<Socket::TimingSocket> ts = Socket::TimingSocket::createTimingSocket(measurement_technique);
    if (measurement_technique == Socket::TimingSocket::KindOfSocket::Kernel) {
        dynamic_cast<Socket::KernelTimingSocket *>(ts.get())->setDevice(interface);
    }else if(measurement_technique == Socket::TimingSocket::KindOfSocket::PCAP) {
        dynamic_cast<Socket::PCAPTimingSocket*>(ts.get())->initPcap(interface);
    }
    ts->connect(host, port);

    for (size_t j=0; j<measurements; j++) {
        for (size_t i = 0; i < SAMPLESIZE; i++) {
            write_buf[0] = i%256;
            ts->write(write_buf, BUFSIZE);
            ssize_t read_size = ts->read(read_buf, BUFSIZE, true);
            times[i].push_back(ts->getLastMeasurement());
            //std::cout << "Measured! "<<times[i][j] << std::endl;
            if (j == 0) {
                correct_case[i] = std::string(read_buf);
            }
        }
    }

    ts->close();

    /* Write timing samples in CSV for analysis with mona timing report */
    ofstream csv_file;
    csv_file.open (outfile);
    size_t ctr = 0;
    for (size_t j=0; j<measurements; j++) {
        for (size_t i = 0; i<SAMPLESIZE; i++) {
            csv_file << ctr << ";" << i << ";" << times[i][j] << "\n";
            ctr++;
        }
    }
    csv_file.close();

    return 0;
}
