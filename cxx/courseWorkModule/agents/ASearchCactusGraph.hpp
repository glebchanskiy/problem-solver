#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchCactusGraph.generated.hpp"

namespace courseWorkNamespace{

class ASearchCactusGraph : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_cactus_graph, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
