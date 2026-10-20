#pragma once
#include "Genome.hpp"
#include <map>
#include <vector>

// The network a genome describes (its phenotype), ready to be evaluated.
class NN
{
  public:
    explicit NN(const Genome &genome);

    // Takes one value per input node, returns one value per output node.
    std::vector<float> activate(const std::vector<float> &inputs);
    // Value of every node after the last activate call.
    const std::map<unsigned int, float> &values() const { return values_; }

  private:
    struct Connection
    {
        unsigned int from;
        float weight;
    };
    struct Neuron
    {
        unsigned int id;
        std::vector<Connection> incoming;
    };

    unsigned int inputs_;
    unsigned int outputs_;
    // Every node with incoming connections, in an order where all sources come first.
    std::vector<Neuron> neurons_;
    std::map<unsigned int, float> values_;
};
