#pragma once
#include "Config.hpp"
#include "Genome.hpp"
#include "InnovationManager.hpp"
#include <functional>
#include <optional>
#include <vector>

struct Species
{
    unsigned int id;
    // Compared against to decide membership: a random member of the previous generation.
    Genome representative;
    // Indices into the population.
    std::vector<std::size_t> members;
    float best_fitness = 0.f;
    unsigned int generations_without_improvement = 0;
};

class Population
{
  public:
    Population(unsigned int inputs, unsigned int outputs, const Config& config = {});

    // Evaluates every genome (fitness has to be >= 0), speciates the population and
    // replaces it with the next generation.
    void epoch(const std::function<float(const Genome&)>& fitness);

    unsigned int generation() const
    {
        return generation_;
    }
    const std::vector<Genome>& genomes() const
    {
        return genomes_;
    }
    // Species of the last evaluated generation.
    const std::vector<Species>& species() const
    {
        return species_;
    }
    // Best genome of the last evaluated generation. Empty before the first epoch.
    const std::optional<Genome>& best() const
    {
        return best_;
    }

  private:
    void speciate();
    // Which species may reproduce this generation.
    std::vector<bool> select_reproducing_species();
    // Number of offspring per species, summing up to the population size.
    std::vector<unsigned int> allocate_offspring(const std::vector<bool>& allowed) const;
    void reproduce(const Species& species, unsigned int offspring, std::vector<Genome>& next);

    Config config_;
    InnovationManager innovations_;
    std::vector<Genome> genomes_;
    std::vector<Species> species_;
    std::optional<Genome> best_;
    float best_fitness_ever_ = 0.f;
    unsigned int generations_without_improvement_ = 0;
    unsigned int generation_ = 0;
    unsigned int next_species_id_ = 0;
};
