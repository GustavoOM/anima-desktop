#include<iostream>
#include <chrono>

using namespace std;

double min(double a, double b){
    if (a <= b)
        return a;
    return b;
}

int round(double n){
    double resto = n - int(n);
    if(resto >= .5)
        return int(n)+1;
    return int(n);
}

int64_t micros() {
    auto ms = std::chrono::duration_cast< std::chrono::microseconds >(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    );
    return ms.count();
}


int main() {

    cout << "MILI: " << round(5.499999) << endl;

}

