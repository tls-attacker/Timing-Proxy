#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "../Socket/ServerSocket/ServerSocket.h"
#include "../helper/helper.h"

 using namespace std::chrono_literals;

#define BUFSIZE (1024)
#define BASESLEEP (100)
#define SLEEPTIMEA (7000)
#define SLEEPTIMEB (0)
#define NOISERANGE (500)



int main(int argc, char const *argv[])
{
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
    ss.bind(1337);
    ss.listen();
    ss.accept();
    try {
        while (true) {
            random_percentage = helper::getRandomPercentage();
            ss.read(read_buf, BUFSIZE);
            sleep_duration_ns = BASESLEEP + (random_percentage*NOISERANGE);
            if (read_buf[0] % 2 == 0) {
                sleep_duration_ns = sleep_duration_ns + SLEEPTIMEA;
                chosen_write_buf = write_buf_a;
            }else{
                sleep_duration_ns = sleep_duration_ns + SLEEPTIMEB;
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
