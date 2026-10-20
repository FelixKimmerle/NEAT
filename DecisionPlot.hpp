#pragma once
#include "Genome.hpp"
#include "Tasks.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

// Shows what a network with two inputs computes: its output over the whole input range as a
// color map, with the task's samples on top (white = 1, black = 0, red outline = misclassified).
class DecisionPlot : public sf::Drawable
{
  public:
    DecisionPlot();
    // Holds a texture that the drawn shape points to, so it must not be copied.
    DecisionPlot(const DecisionPlot&) = delete;
    DecisionPlot& operator=(const DecisionPlot&) = delete;

    void set(const Task& task, const Genome& genome);
    void set_selected(std::size_t sample);
    void set_bounds(const sf::FloatRect& bounds);

  private:
    void rebuild();
    // Position of an input pair inside the plot (y points up).
    sf::Vector2f to_screen(const std::vector<float>& inputs) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr unsigned int resolution = 96;

    std::vector<Sample> samples_;
    std::vector<float> outputs_;
    float input_min_ = 0.f;
    float input_max_ = 1.f;
    std::size_t selected_ = 0;
    sf::FloatRect bounds_;

    sf::Texture texture_;
    sf::RectangleShape background_;
    std::vector<sf::CircleShape> points_;
};
