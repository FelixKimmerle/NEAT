#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
namespace sf
{
class ThickLine : public sf::Drawable
{
  public:
	ThickLine();
	ThickLine(float p_Thicknes, const sf::Vector2f &p_Point1, const sf::Vector2f &p_Point2, const sf::Color &p_Color = sf::Color::Black);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setPoint1(const sf::Vector2f &p_Point1);
	void setPoint2(const sf::Vector2f &p_Point2);
	void setThickness(float p_Thicknes);
	void create(float p_Thicknes, const sf::Vector2f &p_Point1, const sf::Vector2f &p_Point2,const sf::Color &p_Color = sf::Color::Black);
	void setColor(const sf::Color &p_Color);
	sf::Vector2f GetPoint1();
	sf::Vector2f GetPoint2();
	void SetArrowOffset(float p_fValue);
  private:
	sf::Vertex m_Vertices[4];
	sf::Vertex m_TriangleVertices[3];
	float m_Thickness;
	sf::Color m_Color;
	sf::Vector2f m_Point1;
	sf::Vector2f m_Point2;
	float m_fArrowOffset;
};
} // namespace sf