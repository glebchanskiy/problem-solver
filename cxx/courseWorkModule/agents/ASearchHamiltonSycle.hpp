#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchHamiltonSycle.generated.hpp"

namespace courseWorkNamespace{

class ASearchHamiltonSycle : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_hamilton_cycle, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace exampleModule
