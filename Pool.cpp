#include "Pool.hpp"

Pool::Pool(unsigned int p_uiSize) : m_uiSize(p_uiSize)
{
    for (unsigned int i = 0; i < p_uiSize; i++)
    {
        AddGenome(Genome(2, 1, &m_in));
    }
}

Pool::~Pool()
{
}

void Pool::AddGenome(Genome p_Genome)
{
    for (auto &&i : m_Population)
    {
        if (Genome::Distance(i[0], p_Genome) < 0.3)
        {
            i.push_back(p_Genome);
            return;
        }
    }

    std::vector<Genome> newlist(1);
    newlist[0] = p_Genome;
    m_Population.push_back(newlist);
}
