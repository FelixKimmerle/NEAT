#include "Tasks.hpp"
#include "NN.hpp"
#include <algorithm>
#include <cmath>

bool is_correct(float output, const Sample& sample)
{
    return (output >= 0.5f) == (sample.expected == 1.f);
}

float Task::fitness(const Genome& genome) const
{
    // Errors are weighted so that both classes count the same. Otherwise always answering
    // the more common class is a strong local optimum. For XOR (2 vs 2) the weights are 1.
    std::size_t ones = std::count_if(samples.begin(), samples.end(), [](const Sample& s) { return s.expected == 1.f; });
    std::size_t zeros = samples.size() - ones;
    const float one_weight = samples.size() / (2.f * std::max<std::size_t>(ones, 1));
    const float zero_weight = samples.size() / (2.f * std::max<std::size_t>(zeros, 1));

    NN nn(genome);
    float error = 0.f;
    for (const Sample& sample : samples)
    {
        float weight = sample.expected == 1.f ? one_weight : zero_weight;
        error += weight * std::abs(nn.activate(sample.inputs)[0] - sample.expected);
    }
    float score = samples.size() - error;
    return score * score;
}

unsigned int Task::correct_count(const Genome& genome) const
{
    NN nn(genome);
    unsigned int correct = 0;
    for (const Sample& sample : samples)
    {
        if (is_correct(nn.activate(sample.inputs)[0], sample))
        {
            correct++;
        }
    }
    return correct;
}

Task make_xor_task()
{
    return Task{"XOR",
                "Nodes 0 and 1 are the inputs, 2 the output, 3 the bias (blue)",
                {{{0.f, 0.f}, 0.f}, {{0.f, 1.f}, 1.f}, {{1.f, 0.f}, 1.f}, {{1.f, 1.f}, 0.f}},
                0.f,
                1.f,
                Config{}};
}

Task make_circle_task()
{
    const int grid = 5;
    const float radius = 0.8f;
    Task task{"Circle", "Nodes 0 and 1 are x and y, 2 the output (1 = inside), 3 the bias (blue)", {}, -1.f, 1.f,
              Config{}};
    // Combining several hidden nodes needs more connections and larger weights than XOR.
    // 0.3 is the paper's add connection rate for its large population.
    task.config.add_connection_rate = 0.3f;
    task.config.weight_limit = 8.f;
    for (int row = 0; row < grid; row++)
    {
        for (int column = 0; column < grid; column++)
        {
            float x = -1.f + 2.f * column / (grid - 1);
            float y = -1.f + 2.f * row / (grid - 1);
            task.samples.push_back({{x, y}, std::hypot(x, y) < radius ? 1.f : 0.f});
        }
    }
    return task;
}
