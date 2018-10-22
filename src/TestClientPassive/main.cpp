#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "../TimingSocket/TimingSocket.h"
#include "../PcapWrapper/PcapWrapper.h"

using namespace std::chrono_literals;
using namespace std;

#define BUFSIZE 1024
#define SAMPLESIZE 400
#define SAMPLEREPETITIONS 200
#define SERVER_ADDR ("169.254.71.233")
#define SERVER_PORT (1337)

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
    char read_buf[BUFSIZE];
    char write_buf[BUFSIZE];
    memset(write_buf, 0, BUFSIZE);
    // strcpy(write_buf, "Testcase");
    std::string correct_case[SAMPLESIZE];
    uint64_t times[SAMPLESIZE][SAMPLEREPETITIONS];
    uint64_t medians[SAMPLESIZE];
    uint64_t overall_median = 0;
    PcapWrapper pcap("enp2s0");
    pcap.setFilter(SERVER_ADDR, SERVER_PORT);
    pcap.startLoop();
    std::unique_ptr<TimingSocket> ts = TimingSocket::createTimingSocket(TimingSocket::KindOfSocket::PCAP);
    ts->connect(SERVER_ADDR, SERVER_PORT);
    for (size_t i = 0; i<SAMPLESIZE; i++) {
        for (size_t j=0; j<SAMPLEREPETITIONS; j++) {
            ts->write(write_buf, BUFSIZE);
            ssize_t read_size = ts->read(read_buf, BUFSIZE);
            times[i][j] = pcap.timingForPacket(write_buf, BUFSIZE);
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
        if (medians[i] <= overall_median) {
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

    pcap.stopLoop();
    ts->close();

    
	return 0;
}
