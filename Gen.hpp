#pragma once
#include <vector>
#include <ostream>
class Gen
{
  private:
    unsigned int m_uiInnovationID;
    unsigned int m_uiFrom;
    unsigned int m_uiTo;
    float m_fWeight;
    bool m_bEnabled;


  public:
    Gen(unsigned int p_uiInnovationID, unsigned int p_uiFrom, unsigned int p_uiTo, float p_fWeight, bool p_bEnabled = true);
    Gen();
    ~Gen();
    void Disable();
    unsigned int From();
    unsigned int To();
    bool operator==(const Gen& p_Gen);
    unsigned int GetInnovationNumber();
    void SetWeight(float p_fValue);
    void AdjustWeight(float p_fValue);
    float GetWeight();
    bool IsActive();
    friend std::ostream &operator<<(std::ostream &os, const Gen &gen);
};
