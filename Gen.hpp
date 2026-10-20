#pragma once
#include <ostream>

// A connection gene: an edge between two nodes, tagged with its historical marking (innovation number).
class Gen
{
  public:
    Gen(unsigned int innovation, unsigned int from, unsigned int to, float weight, bool enabled = true);

    unsigned int innovation() const
    {
        return innovation_;
    }
    unsigned int from() const
    {
        return from_;
    }
    unsigned int to() const
    {
        return to_;
    }
    float weight() const
    {
        return weight_;
    }
    bool is_enabled() const
    {
        return enabled_;
    }

    void set_weight(float weight)
    {
        weight_ = weight;
    }
    void set_enabled(bool enabled)
    {
        enabled_ = enabled;
    }

    friend std::ostream& operator<<(std::ostream& os, const Gen& gen);

  private:
    unsigned int innovation_;
    unsigned int from_;
    unsigned int to_;
    float weight_;
    bool enabled_;
};
