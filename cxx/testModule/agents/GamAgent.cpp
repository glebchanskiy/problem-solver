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

  
  SC_LOG_DEBUG("Vertex: ");
 	// Создаём итератор c названием it с пятью элементами (0, 1, 2, 3, 4)
 	// 0 - адрес графа
 	// 1 - неизвестная дуга принадлежности
 	// 2 - неизвестная вершина
 	// 3 - неизвестная дуга принадлежности
 	// 4 - ролевое отношение вершина'
	
  ScAddr node = ms_context->GetEdgeTarget(edgeAddr);
  
  
 	ScIterator3Ptr it = ms_context->Iterator3(
 		node,
 		ScType::EdgeAccessConstPosPerm,
 		ScType(0)
 		);

	SC_LOG_DEBUG("test");
	while (it->Next()) {
		SC_LOG_DEBUG("BLYAT");
		node = it->Get(2);
		
        ScType type = ms_context->GetElementType(node);

        if (type.IsNode() == ScType::Node)
        {
            std::string data;
            data = ms_context->HelperGetSystemIdtf(node);
		    SC_LOG_DEBUG(data);
        }
	}

	ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
	ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                                 node,
                                 answer);

	// utils::AgentUtils::finishAgentWork(ms_context.get(), node, answer);
		
    return SC_RESULT_OK;
}

} // namespace exampleModul
  //
