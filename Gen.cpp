#include "Gen.hpp"
#include <iostream>
Gen::Gen(unsigned int p_uiInnovationID, unsigned int p_uiFrom, unsigned int p_uiTo, float p_fWeight, bool p_bEnabled) : m_uiInnovationID(p_uiInnovationID), m_uiFrom(p_uiFrom), m_uiTo(p_uiTo), m_fWeight(p_fWeight), m_bEnabled(p_bEnabled)
{
}

Gen::Gen()
{
}

Gen::~Gen()
{
}

void Gen::Disable()
{
    m_bEnabled = false;
}
unsigned int Gen::From()
{
    return m_uiFrom;
}
unsigned int Gen::To()
{
    return m_uiTo;
}

bool Gen::operator==(const Gen &p_Gen)
{
    return (m_uiFrom == p_Gen.m_uiFrom && m_uiTo == p_Gen.m_uiTo);
}

unsigned int Gen::GetInnovationNumber()
{
    return m_uiInnovationID;
}

void Gen::SetWeight(float p_fValue)
{
    m_fWeight = p_fValue;
}

void Gen::AdjustWeight(float p_fValue)
{
    m_fWeight += p_fValue;
}

float Gen::GetWeight()
{
    return m_fWeight;
}
bool Gen::IsActive()
{
    return m_bEnabled;
}
std::ostream &operator<<(std::ostream &os, const Gen &gen)
{
    os << "(Innov: " << gen.m_uiInnovationID << " " << gen.m_uiFrom << "->" << gen.m_uiTo << " Enabled: " << (gen.m_bEnabled ? "True" : "False") << " Weight: " << gen.m_fWeight << ")";
    return os;
}
