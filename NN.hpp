#pragma once
#include "Genome.hpp"
#include <list>
#include <SFML/Graphics.hpp>
#include "ThickLine.hpp"
class NN : public sf::Drawable
{
  private:
    std::list<unsigned int> m_lOrder;
    void Order(unsigned int p_uiNode, std::list<unsigned int> *p_lOrder, std::set<unsigned int> *Calculated);
    Genome *m_pGenome;
    std::map<unsigned int, std::vector<float>> m_fWeights;

    //for drawing
    std::vector<sf::ThickLine> m_vLines;
    std::vector<sf::CircleShape> m_vNodes;
    std::vector<sf::Text> m_vNodeTexts;
    std::vector<sf::Text> m_vGenTexts;
    std::vector<sf::RectangleShape> m_vRects;
    std::map<unsigned int, unsigned int> m_vLayers;
    void Update();
    unsigned int m_uiMaxLayer;
    static sf::Font s_Font;
    static bool isLoaded;
    float m_fX;
    float m_fY;
    bool m_bDrawGen;
    float m_fScale;
    std::map<unsigned int, unsigned int> ids;
    void Layer(unsigned int p_uiNode, unsigned int *p_Layer);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

  public:
    NN(Genome *p_Genome);
    ~NN();
    std::vector<float> FeedForward(const std::vector<float> &p_Vals);

    //for drawing
    void SetSize(float x, float y);
    void SetScale(float p_fValue);
    void DrawGen(bool p_bValue);
};
