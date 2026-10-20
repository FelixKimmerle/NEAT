#include "Gen.hpp"

Gen::Gen(unsigned int innovation, unsigned int from, unsigned int to, float weight, bool enabled)
    : innovation_(innovation), from_(from), to_(to), weight_(weight), enabled_(enabled)
{
}

std::ostream &operator<<(std::ostream &os, const Gen &gen)
{
    os << "(Innov: " << gen.innovation_ << " " << gen.from_ << "->" << gen.to_
       << " Enabled: " << (gen.enabled_ ? "True" : "False") << " Weight: " << gen.weight_ << ")";
    return os;
}
