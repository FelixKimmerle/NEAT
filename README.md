# NEAT

An implementation of NeuroEvolution of Augmenting Topologies, following
[Stanley & Miikkulainen, "Evolving Neural Networks through Augmenting Topologies" (2002)](https://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf),
with interactive examples drawn with SFML 3.

## Build

Requires CMake 3.24+ and a C++17 compiler. SFML is downloaded and built automatically
(FetchContent), so it does not have to be installed.

    cmake -S . -B build
    cmake --build build

On Linux, building SFML needs the usual system development packages
(X11, Xrandr, Xcursor, Xi, OpenGL, udev).

## Run

    ./build/neat                          # XOR, interactive
    ./build/neat --task circle            # the larger example
    ./build/neat --seed 42                # reproducible run
    ./build/neat --bench 100              # repeat the paper's XOR experiment 100 times
    ./build/neat --task circle --bench 30

### Options

| Option | Description |
|---|---|
| `--task xor\|circle` | Task to start with (default `xor`) |
| `--seed N` | Seed for the random generator, makes a run reproducible |
| `--bench RUNS` | Run the task that many times without a window and print averages |

### Controls

| Key | Action |
|---|---|
| `Right` | Evolve one generation |
| `Enter` | Evolve ten generations |
| `A` | Auto run on/off (one generation every 100 ms) |
| `R` | Start a new run of the current task |
| `T` | Switch task (XOR / Circle) |
| `Up` / `Down` | Pick the sample shown (stops cycling) |
| `C` | Cycle through the samples automatically on/off |
| `Space` | Show innovation numbers and disabled connections |
| Mouse wheel | Zoom |
| `Esc` | Quit |

A run starts paused at generation 1 and stops evolving once the task is solved.

The window shows the best network of the current generation, colored by its activations for
the selected sample, and a plot of what the network computes over the whole input range
(green = 1), with the samples on top (white = 1, black = 0, red outline = misclassified).

## Examples

- **XOR** (section 4.2 of the paper). Needs at least one hidden node.
  Over 100 runs: solved every time, on average after 65 generations with 4.4 hidden nodes
  (paper: 32 generations, 2.35 hidden nodes).
- **Circle**: classify the points of a 5x5 grid as inside or outside a circle. Every sigmoid
  node can only cut the plane along a line, so the network needs several hidden nodes.
  Over 30 runs: 27 solved within 1000 generations, on average after 290 generations with
  5.8 hidden nodes and 27 connections.

## Code

- `Gen` connection gene, `Genome` genotype with mutation, crossover and compatibility distance
- `InnovationManager` historical markings shared by the whole population
- `Population` speciation, explicit fitness sharing, stagnation and reproduction
- `NN` the phenotype (feed-forward evaluation)
- `Tasks` the examples, `NetworkView`, `DecisionPlot`, `ThickLine` drawing
- `Config` all parameters, defaults from section 4.1 of the paper; deviations are commented
