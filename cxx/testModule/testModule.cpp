/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "testModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/GamAgent.hpp"


using namespace testModule;

SC_IMPLEMENT_MODULE(TestModule)

sc_result TestModule::InitializeImpl()
{
  if (!testModule::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(GamAgent)

  return SC_RESULT_OK;
}

sc_result TestModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(GamAgent)

  return SC_RESULT_OK;
}
