/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "courseWorkModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/ASearchHamiltonCycle.hpp"
#include "agents/ASearchBiconnectedGraph.hpp"
#include "agents/ASearchPathInGraph.hpp"
#include "agents/AFindLongestPath.hpp"
#include "agents/ASearchCactusGraph.hpp"
#include "agents/ASearchEdgeConnectivity.hpp"
#include "agents/ASearchMaxAndMiddlePath.hpp"




using namespace courseWorkNamespace;

SC_IMPLEMENT_MODULE(CourseWorkModule)

sc_result CourseWorkModule::InitializeImpl()
{
  if (!courseWorkNamespace::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(ASearchHamiltonCycle)
  SC_AGENT_REGISTER(ASearchBiconnectedGraph)
  SC_AGENT_REGISTER(ASearchPathInGraph)
  SC_AGENT_REGISTER(ASearchCactusGraph)
  SC_AGENT_REGISTER(ASearchEdgeConnectivity)
  SC_AGENT_REGISTER(AFindLongestPath)
  SC_AGENT_REGISTER(ASearchMaxAndMiddlePath)

  return SC_RESULT_OK;
}

sc_result CourseWorkModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(ASearchHamiltonCycle)
  SC_AGENT_UNREGISTER(ASearchBiconnectedGraph)
  SC_AGENT_UNREGISTER(ASearchPathInGraph)
  SC_AGENT_UNREGISTER(ASearchCactusGraph)
  SC_AGENT_UNREGISTER(ASearchEdgeConnectivity)
  SC_AGENT_UNREGISTER(AFindLongestPath)
  SC_AGENT_UNREGISTER(ASearchMaxAndMiddlePath)

  return SC_RESULT_OK;
}
