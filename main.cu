#include <iostream>

#define N 10000000

__global__ void ebat() {

}

int main() {
    ebat<<<1,2>>>();
}
