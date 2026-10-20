#pragma once
#include "Config.hpp"
#include "Genome.hpp"
#include <string>
#include <vector>

struct Sample
{
    std::vector<float> inputs;
    float expected; // 0 or 1
};

// A classification problem with two inputs and one output, learned from a fixed set of samples.
struct Task
{
    std::string name;
    // Explains the node ids shown in the network view.
    std::string legend;
    std::vector<Sample> samples;
    // Range of both inputs, used for plotting.
    float input_min;
    float input_max;
    Config config;

    // The XOR fitness of section 4.2, generalized: (samples - summed error)^2.
    // Every sample's error is weighted so both classes count the same.
    float fitness(const Genome& genome) const;
    float max_fitness() const
    {
        return static_cast<float>(samples.size() * samples.size());
    }
    unsigned int correct_count(const Genome& genome) const;
    bool solved(const Genome& genome) const
    {
        return correct_count(genome) == samples.size();
    }
};

// An output counts as 1 from 0.5 on.
bool is_correct(float output, const Sample& sample);

// The four XOR cases, the experiment of section 4.2 of the paper.
Task make_xor_task();
// Points on a 5x5 grid: is the point inside a circle? Needs several hidden nodes, since every
// sigmoid node can only cut the plane along a straight line and the output has to combine them.
Task make_circle_task();
