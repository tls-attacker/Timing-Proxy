#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "../Socket/ServerSocket/ServerSocket.h"
#include "../helper/helper.h"
#include <boost/program_options.hpp>

 using namespace std::chrono_literals;

#define BUFSIZE (1024)

namespace po = boost::program_options;

int main(int argc, char const *argv[])
{
    int port;
    uint64_t basesleep;
    uint64_t sleeptimea;
    uint64_t sleeptimeb;
    uint64_t noiserange;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("port,p", po::value<int>(&port)->default_value(1337), "set listen port")
            ("basesleep,b", po::value<uint64_t >(&basesleep)->default_value(100), "minimum sleep in any case")
            ("sleeptimea", po::value<uint64_t>(&sleeptimea)->default_value(100), "added sleeptime for a")
            ("sleeptimeb", po::value<uint64_t>(&sleeptimeb)->default_value(0), "added sleeptime for b")
            ("noiserange,n", po::value<uint64_t>(&noiserange)->default_value(500), "range for added noise");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }


    char read_buf[BUFSIZE];
    char write_buf_a[BUFSIZE];
    char write_buf_b[BUFSIZE];
    char* chosen_write_buf;
    double random_percentage;
    uint64_t sleep_duration_ns = 0;
    memset(write_buf_a, 0, BUFSIZE);
    memset(write_buf_b, 0, BUFSIZE);
    write_buf_a[0] = 'a';
    write_buf_b[0] = 'b';
    //helper::initRandomSeed();
    Socket::ServerSocket ss;
    ss.bind(port);
    ss.listen();
    ss.accept();
    try {
        while (true) {
            random_percentage = helper::getRandomPercentage();
            ss.read(read_buf, BUFSIZE);
            sleep_duration_ns = basesleep + (random_percentage*noiserange);
            if (read_buf[0] % 2 == 0) {
                sleep_duration_ns = sleep_duration_ns + sleeptimea;
                chosen_write_buf = write_buf_a;
            }else{
                sleep_duration_ns = sleep_duration_ns + sleeptimeb;
                chosen_write_buf = write_buf_b;
            }
            helper::active_sleep(sleep_duration_ns);
            //helper::passive_sleep(sleep_duration_ns);
            ss.write(chosen_write_buf, BUFSIZE);
            std::cout <<chosen_write_buf[0]<<"slept for "<<sleep_duration_ns<<"ns\n";
            //std::this_thread::yield();
        }
    } catch (std::runtime_error &e) {
        ss.close_client();
        ss.close();
    }
    
	return 0;
}
