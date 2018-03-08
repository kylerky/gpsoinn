#include "gpsoinn.hxx"
#include <cmath>
#include <iostream>
#include <random>

int main() {
    using GPSOINN::GPNet;
    GPNet<0> net(1, 250, 50, 1, 1e-7);

    std::random_device dev;
    std::default_random_engine generator(dev());

    float mean = 1;
    float stddev = 1;
    std::normal_distribution<double> distribution(mean, stddev);
    for (size_t i = 0; i != 10000; ++i) {
        // std::array<float, 1> arr;
        std::vector<double> arr(1);
        arr[0] = distribution(generator);
        net.train(arr);
        // std::cout << net.predict(arr) << std::endl;
    }
    for (double f = -6.0; f <= 6.0; f += 0.1) {
        std::cout << f << ",";
    }
    std::cout << "\n\n" << std::endl;
    for (double f = -6.0; f <= 6.0; f += 0.1) {
        // std::array<float, 1> arr;
        std::vector<double> arr(1);
        arr[0] = f;
        std::cout << net.predict(arr) << ",";
    }
    std::cout << "\n\n" << std::endl;
    for (double f = -6.0; f <= 6.0; f += 0.1) {
        std::cout << std::exp(-(f - mean) * (f - mean) / 2 / stddev / stddev) /
                         std::sqrt(2 * M_PI) / stddev
                  << ",";
    }
    std::cout << std::endl;

    return 0;
}
