#ifndef UTILS_H
#define UTILS_H

#include <random>

namespace Utils {
    template <typename T>
    T getRandom(T min, T max) {
        static std::mt19937 gen(std::random_device{}());
        
        if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(gen);
        } else {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(gen);
        }
    }
}

#endif // UTILS_H
