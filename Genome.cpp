#include "Genome.hpp"
#include "Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
Genome::Genome(unsigned int p_uiInput, unsigned int p_uiOutput, InnovationManager *p_pInnovManager) : m_pInnovManager(p_pInnovManager), m_uiNumInput(p_uiInput), m_uiNumOutput(p_uiOutput)
{
    //m_uiNodes = p_uiInput + p_uiOutput + 1; //1 for Bias
    for (unsigned int i = 0; i < p_uiInput + p_uiOutput + 1; i++)
    {
        m_uiNodes.insert(i);
    }
    m_fFitness = rand() / (float)RAND_MAX;
}

Genome::~Genome()
{
}

void Genome::AddGene(unsigned int p_uiFrom, unsigned int p_uiTo, float p_fWeight)
{
    unsigned int inumber = m_pInnovManager->Register(p_uiFrom, p_uiTo);
    if (m_vInnovationIDs.insert(inumber).second)
    {
        m_uiNodes.insert(p_uiFrom);
        m_uiNodes.insert(p_uiTo);
        m_Gens.push_back(Gen(inumber, p_uiFrom, p_uiTo, p_fWeight));
        m_vInnovGene.insert(std::make_pair(inumber, m_Gens.size() - 1));
        m_uiParents[p_uiTo].push_back(m_Gens.size() - 1);
    }
}
void Genome::AddNode(unsigned int p_uiID)
{
    m_Gens[p_uiID].Disable();
    unsigned int inumber = m_pInnovManager->Register(m_Gens[p_uiID].From(), *m_uiNodes.end() + 1);
    m_uiNodes.insert(*m_uiNodes.end() + 1);
    m_vInnovationIDs.insert(inumber);
    m_Gens.push_back(Gen(inumber, m_Gens[p_uiID].From(), *m_uiNodes.end(), m_Gens[p_uiID].GetWeight()));
    m_uiParents[*m_uiNodes.end()].push_back(m_Gens.size() - 1);
    m_vInnovGene.insert(std::make_pair(inumber, m_Gens.size() - 1));

    inumber = m_pInnovManager->Register(*m_uiNodes.end(), m_Gens[p_uiID].To());
    m_vInnovationIDs.insert(inumber);
    m_Gens.push_back(Gen(inumber, *m_uiNodes.end(), m_Gens[p_uiID].To(), 1.0));
    m_uiParents[m_Gens[p_uiID].To()].push_back(m_Gens.size() - 1);
    m_vInnovGene.insert(std::make_pair(inumber, m_Gens.size() - 1));
    Logger::Log("Mutate Node: " + std::to_string(m_Gens[p_uiID].From()) + " -> " + std::to_string(*m_uiNodes.end()) + " -> " + std::to_string(m_Gens[p_uiID].To()), Logger::Information);
}

std::ostream &operator<<(std::ostream &os, const Genome &genome)
{
    os << "[ Genes: " << genome.m_Gens.size() << " Nodes: " << genome.m_uiNodes.size() << " Fitness: " << genome.m_fFitness << std::endl;
    for (auto x = genome.m_Gens.begin(); x != genome.m_Gens.end(); x++)
    {
        os << "\t" << *x << std::endl;
    }
    os << "]" << std::endl;
    return os;
}

unsigned int RandomNumber()
{
    return (rand() % 100) + 1;
}

float RandomFloat()
{
    return rand() / (float)RAND_MAX;
}

bool Genome::Check(unsigned int p_uiFrom, unsigned int p_uiTo)
{
    if (p_uiFrom >= m_uiNumInput && p_uiFrom < m_uiNumInput + m_uiNumOutput)
    {
        Logger::Log(std::to_string(p_uiFrom) + " is an output node", Logger::Information);
        return false;
    }
    if (p_uiTo < m_uiNumInput)
    {
        Logger::Log(std::to_string(p_uiTo) + " is an input node", Logger::Information);
        return false;
    }
    if (p_uiTo == m_uiNumInput + m_uiNumOutput)
    {
        Logger::Log(std::to_string(p_uiTo) + " is the bias node", Logger::Information);
        return false;
    }
    if (CheckParents(p_uiFrom, p_uiTo))
    {
        Logger::Log(std::to_string(p_uiFrom) + " -> " + std::to_string(p_uiTo) + " Create a Circle", Logger::Warning);
        return false;
    }
    return true;
}

void Genome::Mutate()
{
    if (RandomNumber() < 80 && m_Gens.size() > 0)
    {
        if (RandomNumber() < 90)
        {
            m_Gens[rand() % m_Gens.size()].AdjustWeight((RandomFloat() * 2.f - 1.f) / 20.f);
        }
        else
        {
            m_Gens[rand() % m_Gens.size()].SetWeight(RandomFloat() * 2.f - 1.f);
        }
    }
    else
    {
        if (RandomNumber() < 5)
        {

            unsigned int a = 0;
            unsigned int b = 0;
            while (a == b)
            {
                a = *std::next(m_uiNodes.begin(), rand() % (m_uiNodes.size()));
                b = *std::next(m_uiNodes.begin(), rand() % (m_uiNodes.size()));
            }
            Logger::Log("Try: " + std::to_string(a) + " -> " + std::to_string(b), Logger::Information);
            if (Check(a, b))
            {
                AddGene(a, b);
                Logger::Log("Mutate Gene: " + std::to_string(a) + " -> " + std::to_string(b), Logger::Information);
            }
        }
        if (RandomNumber() < 3 && m_Gens.size() > 0)
        {
            AddNode(rand() % m_Gens.size());
        }
    }
}

bool Genome::CheckParents(unsigned int p_uiNode, unsigned int p_uiTo)
{
    for (auto i = m_uiParents[p_uiNode].begin(); i != m_uiParents[p_uiNode].end(); i++)
    {
        if (m_Gens[*i].From() == p_uiTo)
        {
            return true;
        }
        if (CheckParents(m_Gens[*i].From(), p_uiTo))
        {
            return true;
        }
    }
    return false;
}

void Genome::GetParents(unsigned int p_uiNode, std::vector<unsigned int> *p_Nodes)
{
    for (auto i = m_uiParents[p_uiNode].begin(); i != m_uiParents[p_uiNode].end(); i++)
    {
        p_Nodes->push_back(m_Gens[*i].From());
        GetParents(m_Gens[*i].From(), p_Nodes);
    }
}

Genome Genome::Crossover(const Genome &p_Gen1, const Genome &p_Gen2)
{
    Genome newgenome(p_Gen1.m_uiNumInput, p_Gen1.m_uiNumOutput, p_Gen1.m_pInnovManager);
    std::set<unsigned int> InnovationIDs;
    std::set_union(std::begin(p_Gen1.m_vInnovationIDs), std::end(p_Gen1.m_vInnovationIDs),
                   std::begin(p_Gen2.m_vInnovationIDs), std::end(p_Gen2.m_vInnovationIDs),
                   std::inserter(InnovationIDs, std::begin(InnovationIDs)));
    for (auto i = InnovationIDs.begin(); i != InnovationIDs.end(); i++)
    {
        if (p_Gen1.m_vInnovGene.count(*i) && p_Gen2.m_vInnovGene.count(*i))
        {
            if (rand() % 2)
            {
                unsigned int at = p_Gen1.m_vInnovGene.at(*i);
                Gen g = p_Gen1.m_Gens.at(at);
                newgenome.AddGene(g.From(), g.To(), g.GetWeight());
            }
            else
            {
                unsigned int at = p_Gen2.m_vInnovGene.at(*i);
                Gen g = p_Gen2.m_Gens.at(at);
                newgenome.AddGene(g.From(), g.To(), g.GetWeight());
            }
        }
        else if (p_Gen1.m_vInnovGene.count(*i))
        {
            if (p_Gen1.m_fFitness > p_Gen2.m_fFitness)
            {
                unsigned int at = p_Gen1.m_vInnovGene.at(*i);
                Gen g = p_Gen1.m_Gens.at(at);
                newgenome.AddGene(g.From(), g.To(), g.GetWeight());
            }
        }
        else
        {
            if (p_Gen2.m_fFitness > p_Gen1.m_fFitness)
            {
                unsigned int at = p_Gen2.m_vInnovGene.at(*i);
                Gen g = p_Gen2.m_Gens.at(at);
                newgenome.AddGene(g.From(), g.To(), g.GetWeight());
            }
        }
    }
    return newgenome;
}

float Genome::Distance(Genome &p_Gen1, Genome &p_Gen2)
{
    std::set<unsigned int> InnovationIDs;
    std::set_union(std::begin(p_Gen1.m_vInnovationIDs), std::end(p_Gen1.m_vInnovationIDs),
                   std::begin(p_Gen2.m_vInnovationIDs), std::end(p_Gen2.m_vInnovationIDs),
                   std::inserter(InnovationIDs, std::begin(InnovationIDs)));

    unsigned int gen1max = 0;
    unsigned int gen2max = 0;
    unsigned int excess = 0;
    unsigned int disjoint = 0;
    unsigned int matching = 0;
    unsigned int maxGenSize = std::max(p_Gen1.m_Gens.size(), p_Gen2.m_Gens.size());
    float weightdiff = 0.f;
    for (auto &&i : p_Gen1.m_vInnovationIDs)
    {
        if (i > gen1max)
        {
            gen1max = i;
        }
    }

    for (auto &&i : p_Gen2.m_vInnovationIDs)
    {
        if (i > gen2max)
        {
            gen2max = i;
        }
    }

    for (auto i = InnovationIDs.begin(); i != InnovationIDs.end(); i++)
    {
        if (p_Gen1.m_vInnovGene.count(*i) && p_Gen2.m_vInnovGene.count(*i))
        {
            weightdiff += std::abs(p_Gen1.GetGen(*i).GetWeight() - p_Gen2.GetGen(*i).GetWeight());
            matching++;
        }
        else if (p_Gen1.m_vInnovGene.count(*i) && !p_Gen2.m_vInnovGene.count(*i))
        {
            if (*i > gen2max)
            {
                excess++;
            }
            else
            {
                disjoint++;
            }
        }
        else if (!p_Gen1.m_vInnovGene.count(*i) && p_Gen2.m_vInnovGene.count(*i))
        {
            if (*i > gen1max)
            {
                excess++;
            }
            else
            {
                disjoint++;
            }
        }
    }
    std::cout << "Excess: " << excess << std::endl;
    std::cout << "Disjoint: " << disjoint << std::endl;
    std::cout << "Weight: " << weightdiff / (float)matching << std::endl;
    if (matching == 0)
    {
        matching = 1;
    }
    if(maxGenSize == 0)
    {
        maxGenSize = 1;
    }
    return excess / (float)maxGenSize + disjoint / (float)maxGenSize + weightdiff / (float)matching;
}

std::vector<unsigned int> Genome::GetDirectParents(unsigned int p_uiID)
{
    return m_uiParents[p_uiID];
}

Gen Genome::GetGen(unsigned int p_uiID)
{
    return m_Gens[p_uiID];
}

unsigned int Genome::GetOutputStart()
{
    return m_uiNumInput;
}
unsigned int Genome::GetOutputNum()
{
    return m_uiNumOutput;
}

unsigned int Genome::GetNodeSize()
{
    return m_uiNodes.size();
}
unsigned int Genome::GetGenSize()
{
    return m_Gens.size();
}

std::vector<Gen> Genome::GetGens()
{
    return m_Gens;
}

unsigned int Genome::GetInputNum()
{
    return m_uiNumInput;
}

Genome::Genome()
{

}
