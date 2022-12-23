#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchEdgeConnectivity.generated.hpp"

namespace courseWorkNamespace{

class ASearchEdgeConnectivity : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_edge_connectivity, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
