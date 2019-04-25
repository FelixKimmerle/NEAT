#pragma once
#include <vector>
#include <tuple>
#include "Gen.hpp"
class InnovationManager
{
private:
    unsigned int m_uiCurrentInnovation;
    std::vector<std::tuple<unsigned int,unsigned int>> m_vRegisteredGenes;
public:
    InnovationManager(/* args */);
    ~InnovationManager();
    unsigned int Register(unsigned int p_uiFrom,unsigned int p_uiTo);

};
