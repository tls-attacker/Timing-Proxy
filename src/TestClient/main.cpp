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
#define SAMPLEREPETITIONS 200

void print_array(uint64_t values[], size_t size) {
    std::cout << "[ " <<std::endl;
    for (size_t i = 0; i<size; i++) {
        std::cout << values[i] << ", " << std::endl;
    }
    std::cout << " ]" <<std::endl;
}

uint64_t median(uint64_t values[], size_t size) {
    if (size == 0) {
        throw;
    }
    if (size == 1) {
        return values[0];
    }
    uint64_t* values_sorted = new uint64_t[size];
    memcpy(values_sorted, values, size*sizeof(uint64_t));
    //print_array(values, size);
    // Sort the values
    sort(values_sorted, values_sorted+size);
    //print_array(values_sorted, size);
    // Pick the middle value
    uint64_t med;
    if ((size % 2) != 0) {
        // odd size -> pick middle
        med = values_sorted[size/2];
    }else{
        // even size -> pick both middle points and calculate average
        med =  (values_sorted[size/2] + values_sorted[(size/2)-1]) / 2;
    }
    //std::cout << med << std::endl;
    delete[] values_sorted;
    return med;
}

int main(int argc, char const *argv[])
{
    Socket::TimingSocket::KindOfSocket measurement_technique;
    std::string interface;
    std::string technique;
    std::string host;
    int port;
    std::string outfile;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("technique,t", po::value<std::string>(&technique), "set timing measurement technique to one of CPU, Kernel, PCAP")
            ("interface,i", po::value<std::string>(&interface)->default_value("lo"), "set interface to be used by PCAP")
            ("host", po::value<std::string>(&host)->default_value("127.0.0.1"), "set host of oracle")
            ("port,p", po::value<int>(&port)->default_value(1337), "set port of oracle")
            ("outfile,o", po::value<std::string>(outfile)->default_value("measurements.csv"), "set output csv for mona timing report");
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
    uint64_t times[SAMPLESIZE][SAMPLEREPETITIONS];
    uint64_t medians[SAMPLESIZE];
    uint64_t overall_median = 0;
    std::unique_ptr<Socket::TimingSocket> ts = Socket::TimingSocket::createTimingSocket(measurement_technique);
    if (measurement_technique == Socket::TimingSocket::KindOfSocket::Kernel) {
        dynamic_cast<Socket::KernelTimingSocket *>(ts.get())->setDevice(interface);
    }else if(measurement_technique == Socket::TimingSocket::KindOfSocket::PCAP) {
        dynamic_cast<Socket::PCAPTimingSocket*>(ts.get())->initPcap(interface);
    }
    ts->connect(host, port);
    for (size_t i = 0; i<SAMPLESIZE; i++) {
        for (size_t j=0; j<SAMPLEREPETITIONS; j++) {
            ts->write(write_buf, BUFSIZE);
            ssize_t read_size = ts->read(read_buf, BUFSIZE, true);
            times[i][j] = ts->getLastMeasurement();
            //std::cout << "Measured! "<<times[i][j] << std::endl;
            if (j==0) {
                correct_case[i] = std::string(read_buf);
            }
        }
        medians[i] = median(times[i], SAMPLEREPETITIONS);

        // cout << "Read " << read_size << " Bytes: " << correct_case[i] << endl;
        cout << "Median: " << medians[i] << ", " << std::endl;
        write_buf[0]++;
    }
    overall_median = median(medians, SAMPLESIZE);
    cout << "The median is: " << overall_median << endl;
    size_t success_rate = 0;

    for (size_t i = 0; i<SAMPLESIZE; i++) {
        std::string guessed_case;
        if (medians[i] >= overall_median) {
            guessed_case = "a";
        }else{
            guessed_case = "b";
        }
        if (guessed_case == correct_case[i]) {
            cout << "correctly classified case " << correct_case[i] << ":" << medians[i] << endl;
            success_rate++;
        }else{
            cout << "wrongly classified case " << correct_case[i] << " as " << guessed_case << ":" << medians[i] << endl;
        }
    }

    cout << "The success rate is " << success_rate << "/" << SAMPLESIZE << " (" << ((double)(100*success_rate) / SAMPLESIZE) << "%)" << endl;

    ts->close();

    /* Write timing samples in CSV for analysis with mona timing report */
    ofstream csv_file;
    csv_file.open (outfile);
    csv_file << "Writing this to a file.\n";
    size_t ctr = 0;
    for (size_t j=0; j<SAMPLEREPETITIONS; j++) {
        for (size_t i = 0; i<SAMPLESIZE; i++) {
            csv_file << ctr << ";" << i << ";" << times[i][j] << "\n";
            ctr++;
        }
    }
    csv_file.close();

    return 0;
}
