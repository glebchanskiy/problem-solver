#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "AFindLongestPath.generated.hpp"

namespace courseWorkNamespace{

class AFindLongestPath : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_find_longest_path, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
