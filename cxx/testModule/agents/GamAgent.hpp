#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "GamAgent.generated.hpp"

namespace testModule
{

class GamAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::find_gamelton_cicle, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace exampleModule
