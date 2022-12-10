#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchPathInGraph.generated.hpp"

namespace courseWorkNamespace{

class ASearchPathInGraph : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_search_path_in_graph, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
