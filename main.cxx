#include "gpsoinn.hxx"
#include <cmath>
#include <iostream>
#include <random>

int main() {
    using GPSOINN::GPNet;
    GPNet<1> net(250, 50, 1, 1e-6);

    std::random_device dev;
    std::default_random_engine generator(dev());

    float mean = 0;
    float stddev = 2;
    std::normal_distribution<float> distribution(mean, stddev);
    for (size_t i = 0; i != 10000; ++i) {
        std::array<float, 1> arr;
        arr[0] = distribution(generator);
        net.train(arr);
        // std::cout << net.predict(arr) << std::endl;
    }
    for (float f = -6.0; f <= 6.0; f += 0.1) {
        std::cout << f << ",";
    }
    std::cout << "\n\n" << std::endl;
    for (float f = -6.0; f <= 6.0; f += 0.1) {
        std::array<float, 1> arr;
        arr[0] = f;
        std::cout << net.predict(arr) << ",";
    }
    std::cout << "\n\n" << std::endl;
    for (float f = -6.0; f <= 6.0; f += 0.1) {
        std::cout << std::exp(-(f - mean) * (f - mean) / 2 / stddev / stddev) /
                         std::sqrt(2 * M_PI) / stddev
                  << ",";
    }
    std::cout << std::endl;

    return 0;
}
