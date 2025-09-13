#pragma once
#include <utility>

/**
 * @brief the clamp function
 *
 * @tparam T: it MUST have implemented the comparison operators <, +, >.
 * @param value: the input value
 * @param min: the lowest limit
 * @param max: the hightest limit
 * @return T: the clamped value. output.
 */
template <typename T>
inline T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/**
 * @brief the clamp function with return condition: whether the input value has reaches the limit
 *
 * @tparam T it must have implemented the comparison operators <, +, >.
 * @param value the input value
 * @param min the lowest limit
 * @param max the hightest limit
 * @return pair<T, bool> the first element is the clamped value, the second element is true if the
 * input value has reaches the limit
 */
template <typename T>
inline std::pair<T, bool> clampWithCheck(T value, T min, T max) {
    if (value < min) return {min, true};
    if (value > max) return {max, true};
    return {value, false};
}

template <typename T>
inline std::pair<T, T> clampPair(std::pair<T, T> input, std::pair<T, T> min, std::pair<T, T> max) {
    return std::make_pair(clamp(input.first, min.first, max.first),
                          clamp(input.second, min.second, max.second));
}
