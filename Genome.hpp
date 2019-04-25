#pragma once
#include "Gen.hpp"
#include <map>
#include <set>
#include <ostream>
#include "InnovationManager.hpp"
class Genome
{
private:
  std::vector<Gen> m_Gens;
  std::set<unsigned int> m_vInnovationIDs;
  std::map<unsigned int, unsigned int> m_vInnovGene;
  std::set<unsigned int> m_uiNodes;
  std::map<unsigned int, std::vector<unsigned int>> m_uiParents;
  void GetParents(unsigned int p_uiNode, std::vector<unsigned int> *p_Nodes);
  InnovationManager *m_pInnovManager;
  bool Check(unsigned int p_uiFrom, unsigned int p_uiTo);
  unsigned int m_uiNumInput;
  unsigned int m_uiNumOutput;
  float m_fFitness;

public:
  std::vector<unsigned int> GetDirectParents(unsigned int p_uiID);
  Genome(unsigned int p_uiInput, unsigned int p_uiOutput, InnovationManager *p_pManager);
  Genome();
  ~Genome();
  void AddGene(unsigned int p_uiFrom, unsigned int p_uiTo, float p_fWeight = 1.0);
  void AddNode(unsigned int p_uiID);
  friend std::ostream &operator<<(std::ostream &os, const Genome &genome);
  void Mutate();
  bool CheckParents(unsigned int p_uiNode, unsigned int p_uiTo);
  static Genome Crossover(const Genome &p_Gen1, const Genome &p_Gen2);
  static float Distance(Genome &p_Gen1, Genome &p_Gen2);
  Gen GetGen(unsigned int p_uiID);
  unsigned int GetOutputStart();
  unsigned int GetOutputNum();
  unsigned int GetNodeSize();
  unsigned int GetGenSize();
  std::vector<Gen> GetGens();
  unsigned int GetInputNum();
};
