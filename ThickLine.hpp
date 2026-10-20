#pragma once
#include <SFML/Graphics.hpp>
#include <array>

// A line of arbitrary thickness with an arrow head at its end.
class ThickLine : public sf::Drawable
{
  public:
    // The arrow tip is pulled back from 'to' by arrow_offset, e.g. to stop at the edge of a circle.
    ThickLine(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color, float arrow_offset = 0.f);

  private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    std::array<sf::Vertex, 4> body_;
    std::array<sf::Vertex, 3> arrow_;
};
