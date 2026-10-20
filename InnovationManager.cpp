#include "InnovationManager.hpp"

InnovationManager::InnovationManager(unsigned int first_hidden_node) : next_node_(first_hidden_node)
{
}

unsigned int InnovationManager::connection_innovation(unsigned int from, unsigned int to)
{
    auto [it, inserted] = connections_.try_emplace({from, to}, next_innovation_);
    if (inserted)
    {
        next_innovation_++;
    }
    return it->second;
}

unsigned int InnovationManager::split_node(unsigned int connection_innovation)
{
    auto [it, inserted] = split_nodes_.try_emplace(connection_innovation, next_node_);
    if (inserted)
    {
        next_node_++;
    }
    return it->second;
}

unsigned int InnovationManager::new_node()
{
    return next_node_++;
}
