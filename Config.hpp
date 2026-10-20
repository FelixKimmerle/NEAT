#pragma once

// Parameters of the algorithm. Unless noted otherwise the defaults are the values from section 4.1 of
// Stanley & Miikkulainen, "Evolving Neural Networks through Augmenting Topologies" (2002),
// for the small population (150) experiments.
struct Config
{
    unsigned int population_size = 150;

    // Compatibility distance: delta = c1 * E / N + c2 * D / N + c3 * W
    float excess_coefficient = 1.0f;   // c1
    float disjoint_coefficient = 1.0f; // c2
    float weight_coefficient = 0.4f;   // c3
    float compatibility_threshold = 3.0f;
    // N is set to 1 when both genomes have fewer genes than this.
    unsigned int small_genome_size = 20;

    // Species whose best fitness has not improved for this many generations do not reproduce.
    unsigned int species_stagnation_limit = 15;
    // If the best fitness of the whole population stagnates this long, only the top two species reproduce.
    unsigned int population_stagnation_limit = 20;
    // The champion of each species with more than this many members is copied unchanged.
    unsigned int champion_copy_min_species_size = 5;

    float weight_mutation_rate = 0.8f; // chance that a genome's weights are mutated
    float weight_perturb_rate = 0.9f;  // per weight: perturb, otherwise assign a new random value

    // Not given in the paper; tuned on XOR (100 runs each). With the steep sigmoid, large weights
    // saturate the outputs to exactly 0 or 1 and runs get stuck on a flat fitness plateau
    // (3 of 4 XOR cases right). Capping the weights fixed that, large perturbations help escape it.
    float survival_threshold = 0.2f;      // fraction of each species (best first) that may become parents
    float weight_perturb_strength = 2.5f; // perturbation is uniform in [-strength, strength]
    float weight_range = 2.5f;            // new random weights are uniform in [-range, range]
    float weight_limit = 4.0f;            // weights are clamped to [-limit, limit]

    float add_node_rate = 0.03f;
    float add_connection_rate = 0.05f;
    unsigned int add_connection_attempts = 20;

    float disable_inherited_rate = 0.75f; // chance an inherited gene is disabled if disabled in either parent
    float mutation_only_rate = 0.25f;     // fraction of offspring created without crossover
    float interspecies_mating_rate = 0.001f;
};
