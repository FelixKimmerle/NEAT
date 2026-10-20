#pragma once
#include "Config.hpp"
#include "Gen.hpp"
#include "InnovationManager.hpp"
#include <ostream>
#include <set>
#include <vector>

// Node ids: [0, inputs) are inputs, [inputs, inputs + outputs) are outputs,
// inputs + outputs is the bias (always 1.0) and everything above is a hidden node.
//
// Only feed-forward networks are evolved: no mutation or crossover ever produces
// a cycle of enabled connections.
class Genome
{
  public:
    // Minimal starting structure (section 3.4): every input and the bias is connected
    // directly to every output with a random weight, there are no hidden nodes.
    Genome(unsigned int inputs, unsigned int outputs, InnovationManager &innovations, const Config &config);

    void mutate(InnovationManager &innovations, const Config &config);

    // Matching genes are inherited randomly, disjoint and excess genes from the fitter
    // parent (from both if their fitness is equal). Fitness has to be set on both parents.
    static Genome crossover(const Genome &parent1, const Genome &parent2, const Config &config);
    // Compatibility distance delta, equation (1) of the paper.
    static float distance(const Genome &genome1, const Genome &genome2, const Config &config);

    unsigned int input_count() const { return inputs_; }
    unsigned int output_count() const { return outputs_; }
    unsigned int bias_node() const { return inputs_ + outputs_; }
    bool is_input(unsigned int node) const { return node < inputs_; }
    bool is_output(unsigned int node) const { return node >= inputs_ && node < inputs_ + outputs_; }
    bool is_hidden(unsigned int node) const { return node > bias_node(); }
    unsigned int hidden_count() const { return nodes_.size() - bias_node() - 1; }
    unsigned int enabled_gene_count() const;
    // All nodes ordered so that every enabled connection points forward.
    std::vector<unsigned int> topological_order() const;

    // Sorted by innovation number.
    const std::vector<Gen> &genes() const { return genes_; }
    const std::set<unsigned int> &nodes() const { return nodes_; }

    float fitness() const { return fitness_; }
    void set_fitness(float fitness) { fitness_ = fitness; }

    friend std::ostream &operator<<(std::ostream &os, const Genome &genome);

  private:
    Genome(unsigned int inputs, unsigned int outputs);

    // Adds the gene at its sorted position. If it is enabled and would close a cycle it is added disabled.
    void insert_gene(Gen gene);
    bool has_connection(unsigned int from, unsigned int to) const;
    // Whether an enabled connection from -> to would close a cycle.
    bool creates_cycle(unsigned int from, unsigned int to) const;

    void mutate_weights(const Config &config);
    void mutate_add_connection(InnovationManager &innovations, const Config &config);
    void mutate_add_node(InnovationManager &innovations);

    unsigned int inputs_;
    unsigned int outputs_;
    std::vector<Gen> genes_;
    std::set<unsigned int> nodes_;
    float fitness_ = 0.f;
};
