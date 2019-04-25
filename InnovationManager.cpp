#include "InnovationManager.hpp"
#include <algorithm>
InnovationManager::InnovationManager(/* args */) : m_uiCurrentInnovation(0)
{
}

InnovationManager::~InnovationManager()
{
}

unsigned int InnovationManager::Register(unsigned int p_uiFrom, unsigned int p_uiTo)
{
    auto Found = std::find(m_vRegisteredGenes.begin(), m_vRegisteredGenes.end(), std::tuple<unsigned int, unsigned int>(p_uiFrom, p_uiTo));
    if (Found != m_vRegisteredGenes.end())
    {
        return std::distance(m_vRegisteredGenes.begin(), Found);
    }
    else
    {
        m_vRegisteredGenes.push_back(std::tuple<unsigned int, unsigned int>(p_uiFrom, p_uiTo));
        return m_vRegisteredGenes.size() - 1;
    }
}