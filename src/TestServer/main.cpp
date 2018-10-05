#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <random>
#include "../ServerSocket/ServerSocket.h"

 using namespace std::chrono_literals;

#define BUFSIZE 1024
#define BASESLEEP (100ns)
#define SLEEPTIMEA (0ns)
#define SLEEPTIMEB (2500ns)
#define NOISERANGE (0ns)

void initRandomSeed() {
    srand(time(0));
}

bool randomBool() {
    return rand() % 2 == 0;
}

static std::default_random_engine e;
static std::uniform_real_distribution<> dis(0, 1);

double get_random_percentage() {
    return dis(e);
}

int main(int argc, char const *argv[])
{
    char read_buf[BUFSIZE];
    char write_buf_a[BUFSIZE];
    char write_buf_b[BUFSIZE];
    char* chosen_write_buf;
    double random_percentage;
    std::chrono::duration<double,std::nano> sleep_duration;
    memset(write_buf_a, 0, BUFSIZE);
    memset(write_buf_b, 0, BUFSIZE);
    write_buf_a[0] = 'a';
    write_buf_b[0] = 'b';
    initRandomSeed();
    ServerSocket ss;
    ss.bind(1337);
    ss.listen();
    ss.accept();
    try {
        while (true) {
            random_percentage = get_random_percentage();
            ss.read(read_buf, BUFSIZE);
            sleep_duration = BASESLEEP + (random_percentage*NOISERANGE);
            if (read_buf[0] % 2 == 0) {
                sleep_duration = sleep_duration + SLEEPTIMEA;
                chosen_write_buf = write_buf_a;
            }else{
                sleep_duration = sleep_duration + SLEEPTIMEB;
                chosen_write_buf = write_buf_b;
            }
            std::this_thread::sleep_for(sleep_duration);
            ss.write(chosen_write_buf, BUFSIZE);
        }
    } catch (std::runtime_error e) {
        ss.close_client();
        ss.close();
    }
    
	return 0;
}
