#pragma once
#include <map>
#include <utility>

// Hands out the historical markings (section 3.2 of the paper).
// The same structural mutation always gets the same number, no matter which genome makes it,
// so genes stay comparable across the whole population.
class InnovationManager
{
  public:
    // Node ids below first_hidden_node are reserved for inputs, outputs and the bias.
    explicit InnovationManager(unsigned int first_hidden_node);

    // Innovation number of the connection from -> to.
    unsigned int connection_innovation(unsigned int from, unsigned int to);
    // Id of the node created by splitting the given connection. Splitting the same
    // connection in different genomes yields the same node, so the new genes match up.
    unsigned int split_node(unsigned int connection_innovation);
    // A fresh node id that no genome uses yet.
    unsigned int new_node();

  private:
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> connections_;
    std::map<unsigned int, unsigned int> split_nodes_;
    unsigned int next_innovation_ = 0;
    unsigned int next_node_;
};
