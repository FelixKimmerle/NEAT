#include "Genome.hpp"
#include "Random.hpp"
#include <algorithm>
#include <cmath>
#include <map>

Genome::Genome(unsigned int inputs, unsigned int outputs) : inputs_(inputs), outputs_(outputs)
{
    for (unsigned int node = 0; node <= bias_node(); node++)
    {
        nodes_.insert(node);
    }
}

Genome::Genome(unsigned int inputs, unsigned int outputs, InnovationManager& innovations, const Config& config)
    : Genome(inputs, outputs)
{
    auto connect = [&](unsigned int from, unsigned int to) {
        insert_gene(Gen(innovations.connection_innovation(from, to), from, to,
                        Random::uniform(-config.weight_range, config.weight_range)));
    };
    for (unsigned int to = inputs_; to < inputs_ + outputs_; to++)
    {
        for (unsigned int from = 0; from < inputs_; from++)
        {
            connect(from, to);
        }
        connect(bias_node(), to);
    }
}

unsigned int Genome::enabled_gene_count() const
{
    return std::count_if(genes_.begin(), genes_.end(), [](const Gen& gene) { return gene.is_enabled(); });
}

std::vector<unsigned int> Genome::topological_order() const
{
    // Kahn's algorithm. Enabled connections never form a cycle, so every node ends up in the order.
    std::map<unsigned int, unsigned int> incoming;
    for (unsigned int node : nodes_)
    {
        incoming[node] = 0;
    }
    for (const Gen& gene : genes_)
    {
        if (gene.is_enabled())
        {
            incoming[gene.to()]++;
        }
    }

    std::vector<unsigned int> order;
    for (const auto& [node, count] : incoming)
    {
        if (count == 0)
        {
            order.push_back(node);
        }
    }
    for (std::size_t i = 0; i < order.size(); i++)
    {
        for (const Gen& gene : genes_)
        {
            if (gene.is_enabled() && gene.from() == order[i] && --incoming[gene.to()] == 0)
            {
                order.push_back(gene.to());
            }
        }
    }
    return order;
}

void Genome::insert_gene(Gen gene)
{
    auto position = std::lower_bound(genes_.begin(), genes_.end(), gene.innovation(),
                                     [](const Gen& g, unsigned int innovation) { return g.innovation() < innovation; });
    if (position != genes_.end() && position->innovation() == gene.innovation())
    {
        return;
    }
    if (gene.is_enabled() && creates_cycle(gene.from(), gene.to()))
    {
        gene.set_enabled(false);
    }
    nodes_.insert(gene.from());
    nodes_.insert(gene.to());
    genes_.insert(position, gene);
}

bool Genome::has_connection(unsigned int from, unsigned int to) const
{
    return std::any_of(genes_.begin(), genes_.end(),
                       [&](const Gen& gene) { return gene.from() == from && gene.to() == to; });
}

bool Genome::creates_cycle(unsigned int from, unsigned int to) const
{
    // The new connection closes a cycle if 'from' is already reachable from 'to'.
    std::vector<unsigned int> stack{to};
    std::set<unsigned int> visited;
    while (!stack.empty())
    {
        unsigned int node = stack.back();
        stack.pop_back();
        if (node == from)
        {
            return true;
        }
        if (!visited.insert(node).second)
        {
            continue;
        }
        for (const Gen& gene : genes_)
        {
            if (gene.is_enabled() && gene.from() == node)
            {
                stack.push_back(gene.to());
            }
        }
    }
    return false;
}

void Genome::mutate(InnovationManager& innovations, const Config& config)
{
    if (Random::chance(config.add_node_rate))
    {
        mutate_add_node(innovations);
    }
    if (Random::chance(config.add_connection_rate))
    {
        mutate_add_connection(innovations, config);
    }
    if (Random::chance(config.weight_mutation_rate))
    {
        mutate_weights(config);
    }
}

void Genome::mutate_weights(const Config& config)
{
    for (Gen& gene : genes_)
    {
        if (Random::chance(config.weight_perturb_rate))
        {
            float weight =
                gene.weight() + Random::uniform(-config.weight_perturb_strength, config.weight_perturb_strength);
            gene.set_weight(std::clamp(weight, -config.weight_limit, config.weight_limit));
        }
        else
        {
            gene.set_weight(Random::uniform(-config.weight_range, config.weight_range));
        }
    }
}

void Genome::mutate_add_connection(InnovationManager& innovations, const Config& config)
{
    std::vector<unsigned int> nodes(nodes_.begin(), nodes_.end());
    for (unsigned int attempt = 0; attempt < config.add_connection_attempts; attempt++)
    {
        unsigned int from = nodes[Random::index(nodes.size())];
        unsigned int to = nodes[Random::index(nodes.size())];
        if (from == to || is_output(from) || is_input(to) || to == bias_node() || has_connection(from, to) ||
            creates_cycle(from, to))
        {
            continue;
        }
        insert_gene(Gen(innovations.connection_innovation(from, to), from, to,
                        Random::uniform(-config.weight_range, config.weight_range)));
        return;
    }
}

void Genome::mutate_add_node(InnovationManager& innovations)
{
    std::vector<std::size_t> enabled;
    for (std::size_t i = 0; i < genes_.size(); i++)
    {
        if (genes_[i].is_enabled())
        {
            enabled.push_back(i);
        }
    }
    if (enabled.empty())
    {
        return;
    }

    Gen& split = genes_[enabled[Random::index(enabled.size())]];
    split.set_enabled(false);
    // Copy: insert_gene below invalidates the reference.
    const Gen old = split;

    unsigned int node = innovations.split_node(old.innovation());
    if (nodes_.count(node))
    {
        // This genome already split that connection once (it got re-enabled since).
        node = innovations.new_node();
    }
    // The connection into the new node gets weight 1, the one out of it the old weight,
    // so the network initially behaves almost like before (section 3.1).
    insert_gene(Gen(innovations.connection_innovation(old.from(), node), old.from(), node, 1.f));
    insert_gene(Gen(innovations.connection_innovation(node, old.to()), node, old.to(), old.weight()));
}

Genome Genome::crossover(const Genome& parent1, const Genome& parent2, const Config& config)
{
    const bool equal = parent1.fitness_ == parent2.fitness_;
    const bool first_fitter = parent1.fitness_ > parent2.fitness_;
    const auto& genes1 = parent1.genes_;
    const auto& genes2 = parent2.genes_;

    Genome child(parent1.inputs_, parent1.outputs_);
    auto inherit = [&](Gen gene, bool disabled_in_either_parent) {
        gene.set_enabled(!(disabled_in_either_parent && Random::chance(config.disable_inherited_rate)));
        child.insert_gene(gene);
    };

    std::size_t i = 0;
    std::size_t j = 0;
    while (i < genes1.size() || j < genes2.size())
    {
        bool has1 = i < genes1.size();
        bool has2 = j < genes2.size();
        if (has1 && has2 && genes1[i].innovation() == genes2[j].innovation())
        {
            // Matching gene
            const Gen& chosen = Random::chance(0.5f) ? genes1[i] : genes2[j];
            inherit(chosen, !genes1[i].is_enabled() || !genes2[j].is_enabled());
            i++;
            j++;
        }
        else if (has1 && (!has2 || genes1[i].innovation() < genes2[j].innovation()))
        {
            // Disjoint or excess gene of parent 1
            if (first_fitter || (equal && Random::chance(0.5f)))
            {
                inherit(genes1[i], !genes1[i].is_enabled());
            }
            i++;
        }
        else
        {
            // Disjoint or excess gene of parent 2
            if ((!first_fitter && !equal) || (equal && Random::chance(0.5f)))
            {
                inherit(genes2[j], !genes2[j].is_enabled());
            }
            j++;
        }
    }
    return child;
}

float Genome::distance(const Genome& genome1, const Genome& genome2, const Config& config)
{
    const auto& genes1 = genome1.genes_;
    const auto& genes2 = genome2.genes_;

    std::size_t i = 0;
    std::size_t j = 0;
    unsigned int disjoint = 0;
    unsigned int matching = 0;
    float weight_difference = 0.f;
    while (i < genes1.size() && j < genes2.size())
    {
        if (genes1[i].innovation() == genes2[j].innovation())
        {
            weight_difference += std::abs(genes1[i].weight() - genes2[j].weight());
            matching++;
            i++;
            j++;
        }
        else if (genes1[i].innovation() < genes2[j].innovation())
        {
            disjoint++;
            i++;
        }
        else
        {
            disjoint++;
            j++;
        }
    }
    // Whatever is left lies beyond the other genome's highest innovation number.
    unsigned int excess = (genes1.size() - i) + (genes2.size() - j);

    float n = std::max(genes1.size(), genes2.size());
    if (n < config.small_genome_size)
    {
        n = 1.f;
    }
    float average_weight_difference = matching > 0 ? weight_difference / matching : 0.f;
    return config.excess_coefficient * excess / n + config.disjoint_coefficient * disjoint / n +
           config.weight_coefficient * average_weight_difference;
}

std::ostream& operator<<(std::ostream& os, const Genome& genome)
{
    os << "[ Genes: " << genome.genes_.size() << " Nodes: " << genome.nodes_.size() << " Fitness: " << genome.fitness_
       << std::endl;
    for (const Gen& gene : genome.genes_)
    {
        os << "\t" << gene << std::endl;
    }
    os << "]" << std::endl;
    return os;
}
