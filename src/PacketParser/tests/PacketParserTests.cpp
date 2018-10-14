//
// Created by malte on 14.10.18.
//

#include "PacketParserTests.h"
#include <iostream>

using namespace std;

int main(int argc, const char** argv) {
    PacketParserTests tests;
    if(tests.run_testcases() == PacketParserTests::TESTCASE_AMOUNT) {
        cout << "All tests completed successfully!" << endl;
        return 0;
    }else{
        cout << "Error!" << endl;
        return -1;
    }
}