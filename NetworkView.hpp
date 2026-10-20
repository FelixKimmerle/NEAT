#pragma once
#include "Genome.hpp"
#include "ThickLine.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include <optional>
#include <vector>

// Draws a genome as a layered network: inputs and bias on the left, outputs on the right.
// Connections are green for positive and red for negative weights, nodes are filled by their value.
class NetworkView : public sf::Drawable
{
  public:
    explicit NetworkView(const sf::Font& font);

    void set_genome(const Genome& genome);
    // Node values as returned by NN::values().
    void set_values(const std::map<unsigned int, float>& values);
    void set_bounds(const sf::FloatRect& bounds);
    void set_node_size(float size);
    // Also show disabled connections and label every connection with its innovation number.
    void show_innovations(bool show);

  private:
    void rebuild();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    const sf::Font& font_;
    std::optional<Genome> genome_;
    std::map<unsigned int, float> values_;
    sf::FloatRect bounds_;
    float node_size_ = 25.f;
    bool show_innovations_ = false;

    std::vector<ThickLine> lines_;
    std::vector<sf::RectangleShape> label_boxes_;
    std::vector<sf::Text> labels_;
    std::vector<sf::CircleShape> nodes_;
    std::vector<sf::Text> node_texts_;
};
