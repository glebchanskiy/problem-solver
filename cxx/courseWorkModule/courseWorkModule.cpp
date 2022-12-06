/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "courseWorkModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/ASearchHamiltonCycle.hpp"



using namespace courseWorkNamespace;

SC_IMPLEMENT_MODULE(CourseWorkModule)

sc_result CourseWorkModule::InitializeImpl()
{
  if (!courseWorkNamespace::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(ASearchHamiltonCycle)

  return SC_RESULT_OK;
}

sc_result CourseWorkModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(ASearchHamiltonCycle)

  return SC_RESULT_OK;
}
