#include "NN.hpp"
#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
// The steepened sigmoid from section 4.1 of the paper.
float sigmoid(float x)
{
    return 1.f / (1.f + std::exp(-4.9f * x));
}
} // namespace

NN::NN(const Genome &genome) : inputs_(genome.input_count()), outputs_(genome.output_count())
{
    std::map<unsigned int, std::vector<Connection>> incoming;
    for (const Gen &gene : genome.genes())
    {
        if (gene.is_enabled())
        {
            incoming[gene.to()].push_back({gene.from(), gene.weight()});
        }
    }
    for (unsigned int node : genome.topological_order())
    {
        values_[node] = 0.f;
        auto it = incoming.find(node);
        if (it != incoming.end())
        {
            neurons_.push_back({node, std::move(it->second)});
        }
    }
    values_[genome.bias_node()] = 1.f;
}

std::vector<float> NN::activate(const std::vector<float> &inputs)
{
    if (inputs.size() != inputs_)
    {
        throw std::invalid_argument("NN::activate: expected " + std::to_string(inputs_) + " inputs, got " + std::to_string(inputs.size()));
    }
    for (unsigned int i = 0; i < inputs_; i++)
    {
        values_[i] = inputs[i];
    }
    for (const Neuron &neuron : neurons_)
    {
        float sum = 0.f;
        for (const Connection &connection : neuron.incoming)
        {
            sum += values_[connection.from] * connection.weight;
        }
        values_[neuron.id] = sigmoid(sum);
    }

    std::vector<float> outputs(outputs_);
    for (unsigned int i = 0; i < outputs_; i++)
    {
        outputs[i] = values_[inputs_ + i];
    }
    return outputs;
}
