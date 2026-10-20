#include "ThickLine.hpp"

ThickLine::ThickLine(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color, float arrow_offset)
{
    sf::Vector2f direction = to - from;
    if (direction.lengthSquared() > 0.f)
    {
        direction = direction.normalized();
    }
    const sf::Vector2f normal = direction.perpendicular();
    const sf::Vector2f half_width = normal * (thickness / 2.f);

    const sf::Vector2f tip = to - direction * arrow_offset;
    const float arrow_length = 10.f + 2.f * thickness;
    const sf::Vector2f arrow_base = tip - direction * arrow_length;
    const sf::Vector2f arrow_half_width = normal * (4.f + thickness);

    body_ = {sf::Vertex{from + half_width, color}, sf::Vertex{arrow_base + half_width, color},
             sf::Vertex{arrow_base - half_width, color}, sf::Vertex{from - half_width, color}};
    arrow_ = {sf::Vertex{arrow_base + arrow_half_width, color}, sf::Vertex{tip, color},
              sf::Vertex{arrow_base - arrow_half_width, color}};
}

void ThickLine::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(body_.data(), body_.size(), sf::PrimitiveType::TriangleFan, states);
    target.draw(arrow_.data(), arrow_.size(), sf::PrimitiveType::Triangles, states);
}
