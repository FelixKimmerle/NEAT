#include "NN.hpp"
#include <iostream>

sf::Font NN::s_Font;
bool NN::isLoaded = false;

void NN::Order(unsigned int p_uiNode, std::list<unsigned int> *p_lOrder, std::set<unsigned int> *Calculated)
{
    if (Calculated->insert(p_uiNode).second)
    {
        std::vector<unsigned int> parents = m_pGenome->GetDirectParents(p_uiNode);
        for (auto i = parents.begin(); i != parents.end(); i++)
        {
            if (m_pGenome->GetGen(*i).IsActive())
            {
                Order(m_pGenome->GetGen(*i).From(), p_lOrder, Calculated);
                m_fWeights[p_uiNode].push_back(m_pGenome->GetGen(*i).GetWeight());
            }
        }
        if (p_uiNode != m_pGenome->GetOutputNum() + m_pGenome->GetInputNum() && p_uiNode >= m_pGenome->GetInputNum())
        {
            p_lOrder->push_back(p_uiNode);
        }
    }
}

void NN::Layer(unsigned int p_uiNode, unsigned int *p_Layer)
{
    std::vector<unsigned int> parents = m_pGenome->GetDirectParents(p_uiNode);
    (*p_Layer)++;
    for (auto i = parents.begin(); i != parents.end(); i++)
    {
        if (m_pGenome->GetGen(*i).IsActive())
        {
            Layer(m_pGenome->GetGen(*i).From(), p_Layer);
        }
    }
    (*p_Layer)--;
    m_uiMaxLayer = std::max<unsigned int>(m_uiMaxLayer, *p_Layer);
    m_vLayers[p_uiNode] = std::max<unsigned int>(m_vLayers[p_uiNode], *p_Layer);
}

NN::NN(Genome *p_Genome) : m_pGenome(p_Genome), m_bDrawGen(false), m_fScale(25)
{
    m_uiMaxLayer = 0;
    std::set<unsigned int> Calculated;

    m_vNodes.resize(m_pGenome->GetNodeSize());
    m_vLines.resize(m_pGenome->GetGenSize());
    m_vRects.resize(m_pGenome->GetGenSize());
    m_vGenTexts.resize(m_pGenome->GetGenSize());
    m_vNodeTexts.resize(m_pGenome->GetNodeSize());
    unsigned int layer = 0;
    for (unsigned int node = m_pGenome->GetOutputStart(); node < m_pGenome->GetOutputNum() + m_pGenome->GetOutputStart(); node++)
    {
        Order(node, &m_lOrder, &Calculated);
        Layer(node, &layer);
    }
    for (auto &&i : m_lOrder)
    {
        std::cout << i << " = { ";
        std::vector<unsigned int> parents = m_pGenome->GetDirectParents(i);
        for (auto &&x : parents)
        {
            std::cout << p_Genome->GetGen(x).From() << ",";
        }
        std::cout << " }" << std::endl;
    }

    if (!isLoaded)
    {
        s_Font.loadFromFile("arial.ttf");
        isLoaded = true;
    }

    Update();
}

NN::~NN()
{
}

void NN::SetSize(float x, float y)
{
    m_fX = x;
    m_fY = y;
    Update();
}

void NN::SetScale(float p_fValue)
{
    m_fScale = p_fValue;
    Update();
}

void NN::Update()
{
    unsigned int id = 0;
    if(m_uiMaxLayer == 0)
    {
        m_uiMaxLayer = 2;
    }
    for (unsigned i = 0; i < m_pGenome->GetInputNum(); i++)
    {
        m_vLayers[i] = m_uiMaxLayer;
    }
    m_vLayers[m_pGenome->GetOutputNum() + m_pGenome->GetInputNum()] = m_uiMaxLayer;

    std::map<unsigned int, unsigned int> lsize;
    std::map<unsigned int, unsigned int> lsizeset;

    for (auto &&layer : m_vLayers)
    {
        lsize[layer.second]++;
    }

    for (auto &&layer : m_vLayers)
    {
        unsigned int pos = m_uiMaxLayer - layer.second;

        m_vNodeTexts[id].setFont(s_Font);
        m_vNodeTexts[id].setCharacterSize(m_fScale - 1);
        m_vNodeTexts[id].setFillColor(sf::Color::Black);
        m_vNodeTexts[id].setString(std::to_string(layer.first));
        sf::FloatRect textRect = m_vNodeTexts[id].getLocalBounds();
        m_vNodeTexts[id].setOrigin(textRect.left + textRect.width / 2.0f,
                                   textRect.top + textRect.height / 2.0f);

        m_vNodes[id].setRadius(m_fScale/1.75/*std::max<float>(textRect.width + 3, textRect.height + 3) / 2.f*/);
        m_vNodes[id].setOrigin(m_vNodes[id].getRadius(), m_vNodes[id].getRadius());
        m_vNodes[id].setPosition(((m_fX - m_fScale/1.75*2 - 4) / m_uiMaxLayer) * pos + m_fScale / 1.75 + 2, m_fY / (lsize[layer.second] + 1.f) * ((lsizeset[layer.second]++) + 1.f));
        //m_vNodes[id].setFillColor(sf::Color::White);
        m_vNodes[id].setOutlineColor(sf::Color(150, 150, 150));
        m_vNodes[id].setOutlineThickness(2);

        m_vNodeTexts[id].setPosition(m_vNodes[id].getPosition());
        ids[layer.first] = id;
        id++;
    }
    auto gens = m_pGenome->GetGens();
    id = 0;
    for (auto &&gen : gens)
    {
        if (gen.IsActive())
        {
            sf::Color color = sf::Color::White;
            if (gen.GetWeight() < 0)
            {
                color = sf::Color(255, 255 - (std::min<float>(-gen.GetWeight(), 1.f) * 255.f), 255 - (std::min<float>(-gen.GetWeight(), 1.f) * 255.f));
            }
            else
            {
                color = sf::Color(255 - (std::min<float>(gen.GetWeight(), 1.f) * 255.f), 255, 255 - (std::min<float>(gen.GetWeight(), 1.f) * 255.f));
            }

            m_vLines[id].create(3, m_vNodes[ids[gen.From()]].getPosition(), m_vNodes[ids[gen.To()]].getPosition(), color);
        }
        else
        {
            m_vLines[id].create(1, m_vNodes[ids[gen.From()]].getPosition(), m_vNodes[ids[gen.To()]].getPosition(), sf::Color(150, 150, 150));
        }
        m_vLines[id].SetArrowOffset(m_fScale / 1.75);

        m_vGenTexts[id].setFont(s_Font);
        m_vGenTexts[id].setCharacterSize(m_fScale - 1);
        m_vGenTexts[id].setFillColor(sf::Color::Black);
        m_vGenTexts[id].setString(std::to_string(gen.GetInnovationNumber()));
        sf::FloatRect textRect = m_vGenTexts[id].getLocalBounds();
        m_vGenTexts[id].setOrigin(textRect.left + textRect.width / 2.0f,
                                  textRect.top + textRect.height / 2.0f);
        m_vGenTexts[id].setPosition((m_vLines[id].GetPoint1().x + m_vLines[id].GetPoint2().x) / 2, (m_vLines[id].GetPoint1().y + m_vLines[id].GetPoint2().y) / 2);
        textRect = m_vGenTexts[id].getGlobalBounds();

        m_vRects[id].setPosition(textRect.left - 1, textRect.top - 1);
        m_vRects[id].setOutlineThickness(2);
        m_vRects[id].setOutlineColor(sf::Color(150, 150, 150));
        m_vRects[id].setFillColor(sf::Color::White);
        m_vRects[id].setSize(sf::Vector2f(textRect.width + 2, textRect.height + 2));

        id++;
    }
}

void NN::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (auto &&i : m_vLines)
    {
        target.draw(i, states);
    }
    if (m_bDrawGen)
    {
        for (auto &&i : m_vRects)
        {
            target.draw(i, states);
        }
        for (auto &&i : m_vGenTexts)
        {
            target.draw(i, states);
        }
    }
    for (auto &&i : m_vNodes)
    {
        target.draw(i, states);
    }
    for (auto &&i : m_vNodeTexts)
    {
        target.draw(i, states);
    }
}

void NN::DrawGen(bool p_bValue)
{
    m_bDrawGen = p_bValue;
}

float sigmoid(float x)
{
    return x / (1 + std::abs(x));
}

std::vector<float> NN::FeedForward(const std::vector<float> &p_Vals)
{
    std::map<unsigned int, float> Results;
    for (unsigned int i = 0; i < m_pGenome->GetInputNum(); i++)
    {
        Results[i] = p_Vals[i];
    }
    Results[m_pGenome->GetInputNum() + m_pGenome->GetOutputNum()] = 1.f;
    for (auto &&i : m_lOrder)
    {
        float sum = 0.f;
        auto par = m_pGenome->GetDirectParents(i);
        unsigned int cou = 0;
        for (auto &&p : par)
        {
            if (m_pGenome->GetGen(p).IsActive())
            {
                sum += Results[m_pGenome->GetGen(p).From()] * m_fWeights[i][cou];
                cou++;
            }
        }
        Results[i] = sigmoid(sum);
        
    }
    std::vector<float> Outputs(m_pGenome->GetOutputNum());
    unsigned int c = 0;

    for (auto &&i : Results)
    {
        std::cout << "Node: " << i.first << " = " << i.second << std::endl;
        sf::Color color;
        if (i.second < 0)
        {
            color = sf::Color(255, 255 - (std::min<float>(-i.second, 1.f) * 255.f), 255 - (std::min<float>(-i.second, 1.f) * 255.f));
        }
        else
        {
            color = sf::Color(255 - (std::min<float>(i.second, 1.f) * 255.f), 255, 255 - (std::min<float>(i.second, 1.f) * 255.f));
        }
        m_vNodes[ids[i.first]].setFillColor(color);
    }

    for (unsigned int i = m_pGenome->GetOutputStart(); i < m_pGenome->GetOutputNum() + m_pGenome->GetInputNum(); i++)
    {
        Outputs[c] = Results[i];
        c++;
    }
    return Outputs;
}