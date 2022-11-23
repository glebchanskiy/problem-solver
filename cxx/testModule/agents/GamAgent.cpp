#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_template_search.cpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>

#include "GamAgent.hpp"

using namespace std;
using namespace utils;

namespace testModule
{

SC_AGENT_IMPLEMENTATION(GamAgent)
{

  SC_LOG_DEBUG("INIT Agent: GamAgent");

  if (!edgeAddr.IsValid())
	  return SC_RESULT_ERROR;

  SC_LOG_DEBUG("GamAgent is Valid");

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  SC_LOG_DEBUG(ms_context->GetName());
  
  ScAddr param = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode);

  if (!param.IsValid())
	  return SC_RESULT_ERROR_INVALID_PARAMS;

  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, param);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, Keynodes::find_gamelton_cicle);

  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context.get(), param, Keynodes::find_gamelton_cicle, false);
  while (iterator5->Next())
  {
    ScAddr sheaf = iterator5->Get(0);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, iterator5->Get(1));
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, sheaf);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, iterator5->Get(3));
    GenerationUtils::addSetToOutline(ms_context.get(), sheaf, answer);
  }


  AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);

  

  return SC_RESULT_OK;
}

} // namespace exampleModul
  //
