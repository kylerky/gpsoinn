#include "gpsoinn.hxx"
#include <cmath>
#include <iostream>
#include <random>

int main() {
    using GPSOINN::GPNet;
    GPNet<1> net(250, 50, 1, 0.0001);

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
    for (float f = -6.0; f <= 6.0; f += 0.4) {
        std::array<float, 1> arr;
        arr[0] = f;
        std::cout << f << "\t" << net.predict(arr) << "\t"
                  << std::exp(-(f - mean) * (f - mean) / 2 / stddev / stddev) /
                         std::sqrt(2 * M_PI) / stddev
                  << std::endl;
    }
    // for (float f = -6.0; f <= 6.0; f += 0.4) {
    //     std::cout << f << "\t"
    //               << std::exp(-(f - mean) * (f - mean) / 2 / stddev / stddev)
    //               /
    //                      std::sqrt(2 * M_PI) / stddev
    //               << std::endl;
    // }

    return 0;
}
