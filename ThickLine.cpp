#include "ThickLine.hpp"
namespace sf
{
ThickLine::ThickLine()
{
}

ThickLine::ThickLine(float p_Thicknes, const sf::Vector2f &p_Point1, const sf::Vector2f &p_Point2, const sf::Color &p_Color) : m_Color(p_Color), m_Thickness(p_Thicknes), m_Point1(p_Point1), m_Point2(p_Point2)
{
    sf::Vector2f direction = m_Point2 - m_Point1;
    sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (m_Thickness / 2.f) * unitPerpendicular;

    m_Vertices[0].position = m_Point1 + offset;
    m_Vertices[1].position = m_Point2 + offset;
    m_Vertices[2].position = m_Point2 - offset;
    m_Vertices[3].position = m_Point1 - offset;


    m_TriangleVertices[0].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) + sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[1].position = m_Point2 - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[2].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) - sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);

    m_TriangleVertices[0].color = m_Color;
    m_TriangleVertices[1].color = m_Color;
    m_TriangleVertices[2].color = m_Color;

    for (int i = 0; i < 4; ++i)
    {
        m_Vertices[i].color = m_Color;
    }
}

void ThickLine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_Vertices, 4, sf::Quads, states);
    target.draw(m_TriangleVertices, 3, sf::Triangles, states);
}

void ThickLine::setPoint1(const sf::Vector2f &p_Point1)
{
    sf::Vector2f direction = m_Point2 - p_Point1;
    sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (m_Thickness / 2.f) * unitPerpendicular;

    m_Vertices[0].position = p_Point1 + offset;
    m_Vertices[1].position = m_Point2 + offset;
    m_Vertices[2].position = m_Point2 - offset;
    m_Vertices[3].position = p_Point1 - offset;


    m_TriangleVertices[0].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) + sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[1].position = m_Point2 - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[2].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) - sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);

    m_TriangleVertices[0].color = m_Color;
    m_TriangleVertices[1].color = m_Color;
    m_TriangleVertices[2].color = m_Color;
}

void ThickLine::setPoint2(const sf::Vector2f &p_Point2)
{
    sf::Vector2f direction = p_Point2 - m_Point1;
    sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (m_Thickness / 2.f) * unitPerpendicular;

    m_Vertices[0].position = m_Point1 + offset;
    m_Vertices[1].position = p_Point2 + offset;
    m_Vertices[2].position = p_Point2 - offset;
    m_Vertices[3].position = m_Point1 - offset;


    m_TriangleVertices[0].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) + sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[1].position = m_Point2 - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[2].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) - sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);

    m_TriangleVertices[0].color = m_Color;
    m_TriangleVertices[1].color = m_Color;
    m_TriangleVertices[2].color = m_Color;
}

void ThickLine::setThickness(float p_Thicknes)
{
    sf::Vector2f direction = m_Point2 - m_Point1;
    sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (m_Thickness / 2.f) * unitPerpendicular;

    m_TriangleVertices[0].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) + sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[1].position = m_Point2 - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[2].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) - sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);

    m_TriangleVertices[0].color = m_Color;
    m_TriangleVertices[1].color = m_Color;
    m_TriangleVertices[2].color = m_Color;
}
void ThickLine::create(float p_Thicknes, const sf::Vector2f &p_Point1, const sf::Vector2f &p_Point2, const sf::Color &p_Color)
{
    m_Thickness = p_Thicknes;
    m_Point1 = p_Point1;
    m_Point2 = p_Point2;
    m_Color = p_Color;

    sf::Vector2f direction = m_Point2 - m_Point1;
    sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

    sf::Vector2f offset = (m_Thickness / 2.f) * unitPerpendicular;

    m_Vertices[0].position = m_Point1 + offset;
    m_Vertices[1].position = m_Point2 + offset;
    m_Vertices[2].position = m_Point2 - offset;
    m_Vertices[3].position = m_Point1 - offset;

    m_TriangleVertices[0].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) + sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[1].position = m_Point2 - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);
    m_TriangleVertices[2].position = m_Point2 - sf::Vector2f(unitDirection.x * 20.f, unitDirection.y * 20.f) - sf::Vector2f(offset.x * 4, offset.y * 4) - sf::Vector2f(unitDirection.x * m_fArrowOffset, unitDirection.y * m_fArrowOffset);

    m_TriangleVertices[0].color = p_Color;
    m_TriangleVertices[1].color = p_Color;
    m_TriangleVertices[2].color = p_Color;

    for (int i = 0; i < 4; ++i)
    {
        m_Vertices[i].color = m_Color;
    }
}
void ThickLine::setColor(const sf::Color &p_Color)
{
    m_Color = p_Color;
}

sf::Vector2f ThickLine::GetPoint1()
{
    return m_Point1;
}
sf::Vector2f ThickLine::GetPoint2()
{
    return m_Point2;
}

void ThickLine::SetArrowOffset(float p_fValue)
{
    m_fArrowOffset = p_fValue;
}

} // namespace sf