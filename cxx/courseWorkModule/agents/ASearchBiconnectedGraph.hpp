#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchBiconnectedGraph.generated.hpp"

namespace courseWorkNamespace{

class ASearchBiconnectedGraph : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_biconnected_graph, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
