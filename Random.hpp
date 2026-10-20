#pragma once
#include <cstddef>
#include <random>

// One shared engine instead of rand(), so a whole run is reproducible from a single seed.
namespace Random
{
inline std::mt19937& engine()
{
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

inline void seed(unsigned int value)
{
    engine().seed(value);
}

// Uniform float in [min, max).
inline float uniform(float min = 0.f, float max = 1.f)
{
    return std::uniform_real_distribution<float>(min, max)(engine());
}

// True with the given probability.
inline bool chance(float probability)
{
    return uniform() < probability;
}

// Uniform index in [0, size). size must be > 0.
inline std::size_t index(std::size_t size)
{
    return std::uniform_int_distribution<std::size_t>(0, size - 1)(engine());
}
} // namespace Random
