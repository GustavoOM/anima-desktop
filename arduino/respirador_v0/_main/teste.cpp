#include<iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
int main() {
    milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );


    cout << "MILI: " << ms.count() << endl;
}