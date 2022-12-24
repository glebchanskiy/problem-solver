#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "ASearchMaxAndMiddlePath.generated.hpp"

namespace courseWorkNamespace{

class ASearchMaxAndMiddlePath : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_find_max_and_middle_path, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace courseWorkNamespace
