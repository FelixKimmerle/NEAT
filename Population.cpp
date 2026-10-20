#include "Population.hpp"
#include "Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

Population::Population(unsigned int inputs, unsigned int outputs, const Config& config)
    : config_(config), innovations_(inputs + outputs + 1)
{
    // Uniform minimal population (section 3.4): same topology, different random weights.
    for (unsigned int i = 0; i < config_.population_size; i++)
    {
        genomes_.emplace_back(inputs, outputs, innovations_, config_);
    }
}

void Population::epoch(const std::function<float(const Genome&)>& fitness)
{
    for (Genome& genome : genomes_)
    {
        genome.set_fitness(fitness(genome));
    }

    const Genome& best = *std::max_element(genomes_.begin(), genomes_.end(),
                                           [](const Genome& a, const Genome& b) { return a.fitness() < b.fitness(); });
    best_ = best;
    if (best.fitness() > best_fitness_ever_)
    {
        best_fitness_ever_ = best.fitness();
        generations_without_improvement_ = 0;
    }
    else
    {
        generations_without_improvement_++;
    }

    speciate();

    std::vector<unsigned int> offspring = allocate_offspring(select_reproducing_species());
    std::vector<Genome> next;
    next.reserve(config_.population_size);
    for (std::size_t s = 0; s < species_.size(); s++)
    {
        reproduce(species_[s], offspring[s], next);
    }

    // Species without offspring die out. The others are represented in the next
    // generation by a random member of this one.
    std::vector<Species> surviving;
    for (std::size_t s = 0; s < species_.size(); s++)
    {
        if (offspring[s] > 0)
        {
            Species& species = species_[s];
            species.representative = genomes_[species.members[Random::index(species.members.size())]];
            surviving.push_back(std::move(species));
        }
    }
    species_ = std::move(surviving);
    genomes_ = std::move(next);
    generation_++;
}

void Population::speciate()
{
    for (Species& species : species_)
    {
        species.members.clear();
    }
    for (std::size_t i = 0; i < genomes_.size(); i++)
    {
        // Placed into the first compatible species, so species never overlap.
        auto compatible = std::find_if(species_.begin(), species_.end(), [&](const Species& species) {
            return Genome::distance(genomes_[i], species.representative, config_) < config_.compatibility_threshold;
        });
        if (compatible != species_.end())
        {
            compatible->members.push_back(i);
        }
        else
        {
            species_.push_back(Species{next_species_id_++, genomes_[i], {i}});
        }
    }
    species_.erase(std::remove_if(species_.begin(), species_.end(),
                                  [](const Species& species) { return species.members.empty(); }),
                   species_.end());

    for (Species& species : species_)
    {
        float best = 0.f;
        for (std::size_t member : species.members)
        {
            best = std::max(best, genomes_[member].fitness());
        }
        if (best > species.best_fitness)
        {
            species.best_fitness = best;
            species.generations_without_improvement = 0;
        }
        else
        {
            species.generations_without_improvement++;
        }
    }
}

std::vector<bool> Population::select_reproducing_species()
{
    std::vector<bool> allowed(species_.size());
    auto allow_top_two = [&]() {
        std::vector<std::size_t> order(species_.size());
        std::iota(order.begin(), order.end(), 0);
        std::stable_sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
            return species_[a].best_fitness > species_[b].best_fitness;
        });
        for (std::size_t i = 0; i < std::min<std::size_t>(2, order.size()); i++)
        {
            allowed[order[i]] = true;
            species_[order[i]].generations_without_improvement = 0;
        }
    };

    if (generations_without_improvement_ >= config_.population_stagnation_limit && species_.size() > 2)
    {
        // The whole population is stuck: refocus the search on the two best species.
        generations_without_improvement_ = 0;
        allow_top_two();
        return allowed;
    }

    for (std::size_t s = 0; s < species_.size(); s++)
    {
        allowed[s] = species_[s].generations_without_improvement < config_.species_stagnation_limit;
    }
    if (std::none_of(allowed.begin(), allowed.end(), [](bool a) { return a; }))
    {
        // Every species is stagnant. The paper does not say what happens then; give the best two another chance.
        allow_top_two();
    }
    return allowed;
}

std::vector<unsigned int> Population::allocate_offspring(const std::vector<bool>& allowed) const
{
    // Explicit fitness sharing (equation 2): the adjusted fitness of a member is its fitness divided
    // by the species size, so a species' share is proportional to its average fitness.
    std::vector<float> shares(species_.size(), 0.f);
    for (std::size_t s = 0; s < species_.size(); s++)
    {
        if (!allowed[s])
        {
            continue;
        }
        float sum = 0.f;
        for (std::size_t member : species_[s].members)
        {
            sum += genomes_[member].fitness();
        }
        shares[s] = sum / species_[s].members.size();
    }
    float total = std::accumulate(shares.begin(), shares.end(), 0.f);
    if (total <= 0.f)
    {
        // Nobody has any fitness: split evenly between the allowed species.
        for (std::size_t s = 0; s < species_.size(); s++)
        {
            shares[s] = allowed[s] ? 1.f : 0.f;
        }
        total = std::accumulate(shares.begin(), shares.end(), 0.f);
    }

    // Round down, then hand the leftover slots to the largest remainders.
    std::vector<unsigned int> offspring(species_.size());
    std::vector<std::pair<float, std::size_t>> remainders;
    unsigned int assigned = 0;
    for (std::size_t s = 0; s < species_.size(); s++)
    {
        float exact = shares[s] / total * config_.population_size;
        offspring[s] = static_cast<unsigned int>(exact);
        assigned += offspring[s];
        remainders.emplace_back(exact - offspring[s], s);
    }
    std::sort(remainders.rbegin(), remainders.rend());
    for (std::size_t i = 0; assigned < config_.population_size; i++, assigned++)
    {
        offspring[remainders[i % remainders.size()].second]++;
    }
    return offspring;
}

void Population::reproduce(const Species& species, unsigned int offspring, std::vector<Genome>& next)
{
    if (offspring == 0)
    {
        return;
    }
    std::vector<const Genome*> members;
    for (std::size_t member : species.members)
    {
        members.push_back(&genomes_[member]);
    }
    std::sort(members.begin(), members.end(),
              [](const Genome* a, const Genome* b) { return a->fitness() > b->fitness(); });

    if (members.size() > config_.champion_copy_min_species_size)
    {
        next.push_back(*members.front());
        offspring--;
    }

    // Only the best members get to reproduce.
    std::size_t parent_count = std::max<std::size_t>(1, std::ceil(members.size() * config_.survival_threshold));
    members.resize(parent_count);

    for (unsigned int i = 0; i < offspring; i++)
    {
        const Genome& mother = *members[Random::index(members.size())];
        if (members.size() == 1 || Random::chance(config_.mutation_only_rate))
        {
            next.push_back(mother);
        }
        else
        {
            const Genome* father = members[Random::index(members.size())];
            if (Random::chance(config_.interspecies_mating_rate))
            {
                father = &genomes_[Random::index(genomes_.size())];
            }
            next.push_back(Genome::crossover(mother, *father, config_));
        }
        next.back().mutate(innovations_, config_);
    }
}
