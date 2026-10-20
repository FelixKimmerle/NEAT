#include "DecisionPlot.hpp"
#include "Logger/Logger.hpp"
#include "NN.hpp"
#include "NetworkView.hpp"
#include "Population.hpp"
#include "Random.hpp"
#include "Tasks.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
// Repeats an experiment and prints averages. For XOR these compare to section 4.2 of the paper.
int benchmark(const Task &task, unsigned int runs)
{
    const unsigned int max_generations = 1000;
    const Config &config = task.config;
    auto fitness = [&](const Genome &genome) { return task.fitness(genome); };
    unsigned int failures = 0;
    double generations = 0, hidden = 0, genes = 0;
    for (unsigned int run = 0; run < runs; run++)
    {
        Population population(2, 1, config);
        bool solved = false;
        while (!solved && population.generation() < max_generations)
        {
            population.epoch(fitness);
            solved = task.solved(*population.best());
        }
        if (!solved)
        {
            failures++;
            continue;
        }
        generations += population.generation();
        hidden += population.best()->hidden_count();
        genes += population.best()->enabled_gene_count();
    }

    const bool xor_task = task.name == "XOR";
    auto paper = [&](const char *value) { return xor_task ? std::string("   (paper: ") + value + ")" : std::string(); };
    unsigned int solved_runs = runs - failures;
    std::cout << task.name << ": " << runs << " runs, failed: " << failures << " (limit " << max_generations << " generations)\n";
    if (solved_runs > 0)
    {
        std::cout << std::fixed << std::setprecision(2)
                  << "Average generations:      " << generations / solved_runs << paper("32") << "\n"
                  << "Average evaluations:      " << generations / solved_runs * config.population_size << paper("4755") << "\n"
                  << "Average hidden nodes:     " << hidden / solved_runs << paper("2.35") << "\n"
                  << "Average enabled genes:    " << genes / solved_runs << paper("7.48") << "\n";
    }
    return failures == 0 ? 0 : 1;
}

// Evolves a task step by step and shows the best network of the current generation.
class Demo
{
  public:
    Demo(const sf::Font &font, const Task &task) : view_(font), hud_(font, "", 17), help_(font, "", 15)
    {
        hud_.setFillColor(sf::Color::White);
        hud_.setPosition({15.f, 10.f});
        help_.setFillColor(sf::Color(170, 170, 170));
        help_.setString("[Right] 1 generation   [Enter] 10 generations   [A] auto run on/off   [R] new run   [T] next task\n"
                        "[Up]/[Down] pick sample   [C] cycle samples on/off   [Space] innovation numbers   [Wheel] zoom   [Esc] quit");
        set_task(task);
    }

    void set_task(const Task &task)
    {
        task_ = task;
        restart();
    }

    void restart()
    {
        population_.emplace(2, 1, task_.config);
        solved_ = false;
        auto_run_ = false;
        current_sample_ = 0;
        step(1);
    }

    // Evolves up to the given number of generations, stopping early once the task is solved.
    void step(unsigned int generations)
    {
        for (unsigned int i = 0; i < generations && !solved_; i++)
        {
            population_->epoch([this](const Genome &genome) { return task_.fitness(genome); });
            solved_ = task_.solved(*population_->best());
        }
        if (solved_)
        {
            auto_run_ = false;
        }
        view_.set_genome(*population_->best());
        plot_.set(task_, *population_->best());
        refresh();
    }

    void select_sample(int offset)
    {
        const int count = static_cast<int>(task_.samples.size());
        current_sample_ = static_cast<std::size_t>(((static_cast<int>(current_sample_) + offset) % count + count) % count);
        refresh();
    }

    void toggle_auto_run() { auto_run_ = !auto_run_ && !solved_; refresh(); }
    void toggle_cycling() { cycle_samples_ = !cycle_samples_; }

    bool auto_run() const { return auto_run_; }
    bool cycle_samples() const { return cycle_samples_; }
    const Task &task() const { return task_; }
    NetworkView &view() { return view_; }

    void set_window_size(sf::Vector2u size)
    {
        const float plot_size = 300.f;
        plot_.set_bounds({{size.x - plot_size - 20.f, 15.f}, {plot_size, plot_size}});
        view_.set_bounds({{40.f, plot_size + 40.f}, {size.x - 80.f, std::max(size.y - plot_size - 110.f, 50.f)}});
        help_.setPosition({15.f, size.y - 50.f});
    }

    void draw(sf::RenderTarget &target) const
    {
        target.draw(view_);
        target.draw(plot_);
        target.draw(hud_);
        target.draw(help_);
    }

  private:
    // Updates the text, the plot and the node colors for the current generation and sample.
    void refresh()
    {
        const Genome &best = *population_->best();
        NN nn(best);

        std::ostringstream hud;
        hud << "Task: " << task_.name << "   Generation " << population_->generation() << "   ";
        if (solved_)
        {
            hud << "SOLVED - press R for a new run";
        }
        else
        {
            hud << (auto_run_ ? "running" : "paused");
        }
        hud << std::fixed << std::setprecision(3) << "\nBest network: fitness " << best.fitness() << " / " << std::setprecision(0)
            << task_.max_fitness() << ", " << best.hidden_count() << " hidden nodes, " << best.enabled_gene_count()
            << " enabled connections\n";

        hud << "Species (" << population_->species().size() << "): ";
        for (const Species &species : population_->species())
        {
            hud << species.members.size() << " ";
        }
        hud << "\n\n" << task_.legend << "\n";

        hud << "Correctly classified: " << task_.correct_count(best) << " / " << task_.samples.size() << "\n";
        const Sample &sample = task_.samples[current_sample_];
        float output = nn.activate(sample.inputs)[0];
        hud << std::defaultfloat << "Sample " << current_sample_ + 1 << ": (" << sample.inputs[0] << ", " << sample.inputs[1]
            << ")  ->  " << std::fixed << std::setprecision(3) << output << "   expected " << std::setprecision(0)
            << sample.expected << (is_correct(output, sample) ? "   right" : "   WRONG") << "\n";
        hud_.setString(hud.str());

        view_.set_values(nn.values());
        plot_.set_selected(current_sample_);
    }

    Task task_;
    std::optional<Population> population_;
    NetworkView view_;
    DecisionPlot plot_;
    sf::Text hud_;
    sf::Text help_;
    bool solved_ = false;
    bool auto_run_ = false;
    bool cycle_samples_ = true;
    std::size_t current_sample_ = 0;
};

int run_gui(const std::vector<Task> &tasks, std::size_t task)
{
    sf::Font font;
    if (!font.openFromFile(NEAT_FONT_PATH))
    {
        Logger::log(std::string("Could not load ") + NEAT_FONT_PATH, Logger::Error);
        return 1;
    }

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({1400, 900}), "NEAT", sf::Style::Default, sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

    Demo demo(font, tasks[task]);
    demo.set_window_size(window.getSize());
    float node_size = 30.f;
    demo.view().set_node_size(node_size);
    bool show_innovations = false;

    const sf::Time generation_interval = sf::milliseconds(100);
    const sf::Time sample_interval = sf::milliseconds(800);
    sf::Clock generation_clock;
    sf::Clock sample_clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *key = event->getIf<sf::Event::KeyPressed>())
            {
                switch (key->code)
                {
                case sf::Keyboard::Key::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Key::Right:
                    demo.step(1);
                    break;
                case sf::Keyboard::Key::Enter:
                    demo.step(10);
                    break;
                case sf::Keyboard::Key::A:
                    demo.toggle_auto_run();
                    break;
                case sf::Keyboard::Key::R:
                    demo.restart();
                    break;
                case sf::Keyboard::Key::T:
                    task = (task + 1) % tasks.size();
                    demo.set_task(tasks[task]);
                    break;
                case sf::Keyboard::Key::C:
                    demo.toggle_cycling();
                    break;
                case sf::Keyboard::Key::Up:
                case sf::Keyboard::Key::Down:
                    if (demo.cycle_samples())
                    {
                        demo.toggle_cycling();
                    }
                    demo.select_sample(key->code == sf::Keyboard::Key::Up ? -1 : 1);
                    break;
                case sf::Keyboard::Key::Space:
                    show_innovations = !show_innovations;
                    demo.view().show_innovations(show_innovations);
                    break;
                default:
                    break;
                }
            }
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(resized->size))));
                demo.set_window_size(resized->size);
            }
            else if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                node_size = std::clamp(node_size + wheel->delta * 2.f, 10.f, 80.f);
                demo.view().set_node_size(node_size);
            }
        }

        if (demo.auto_run() && generation_clock.getElapsedTime() > generation_interval)
        {
            demo.step(1);
            generation_clock.restart();
        }
        if (demo.cycle_samples() && sample_clock.getElapsedTime() > sample_interval)
        {
            demo.select_sample(1);
            sample_clock.restart();
        }

        window.clear(sf::Color(30, 30, 30));
        demo.draw(window);
        window.display();
    }
    return 0;
}

int usage(const char *program)
{
    std::cerr << "Usage: " << program << " [--task xor|circle] [--seed N] [--bench RUNS]" << std::endl;
    return 1;
}
} // namespace

int main(int argc, char **argv)
{
    const std::vector<Task> tasks{make_xor_task(), make_circle_task()};
    std::size_t task = 0;
    unsigned int seed = std::random_device{}();
    unsigned int bench_runs = 0;
    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc)
        {
            seed = std::stoul(argv[++i]);
        }
        else if (std::strcmp(argv[i], "--bench") == 0 && i + 1 < argc)
        {
            bench_runs = std::stoul(argv[++i]);
        }
        else if (std::strcmp(argv[i], "--task") == 0 && i + 1 < argc)
        {
            std::string name = argv[++i];
            auto found = std::find_if(tasks.begin(), tasks.end(), [&](const Task &t) {
                std::string lower = t.name;
                std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });
                return lower == name;
            });
            if (found == tasks.end())
            {
                return usage(argv[0]);
            }
            task = found - tasks.begin();
        }
        else
        {
            return usage(argv[0]);
        }
    }
    Random::seed(seed);
    Logger::log("Seed: " + std::to_string(seed), Logger::Information);

    return bench_runs > 0 ? benchmark(tasks[task], bench_runs) : run_gui(tasks, task);
}
