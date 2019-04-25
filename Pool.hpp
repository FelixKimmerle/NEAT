#pragma once

#include "NN.hpp"

class Pool
{
private:
    std::vector<std::vector<Genome>> m_Population;
    unsigned int m_uiSize;
    void AddGenome(Genome p_Genome);
    InnovationManager m_in;
public:
    Pool(unsigned int p_uiSize);
    ~Pool();
};
