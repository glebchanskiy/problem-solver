#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchHamiltonCycle.generated.hpp"

namespace courseWorkNamespace{

class ASearchHamiltonCycle : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_hamilton_cycle, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
