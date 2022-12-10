#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include "ASearchBiconnectedGraph.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{
  
void generateAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr startNode, ScAddr structNode, bool isBiconnected) {
        ScAddr answer = context->CreateNode(ScType::NodeConst);
        ScAddr edge = context->CreateEdge(ScType::EdgeDCommonConst, startNode, answer);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, courseWorkNamespace::Keynodes::nrel_answer, edge);
        
        context->HelperSetSystemIdtf("'" + context->HelperGetSystemIdtf(structNode) + "' is " +
                     (isBiconnected ? "biconnecteed" : "not biconnected"), answer);
}


vector<ScAddr> getVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode) {
				vector<ScAddr> allVertexes;
				ScIterator3Ptr it = context->Iterator3(structNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

				while (it->Next()) {
							ScAddr node = it->Get(2);
							ScType node_type = context->GetElementType(node);

							if (node_type.IsNode() == ScType::Node) {
									allVertexes.push_back(node);
							}
				}
				return allVertexes;
}


bool isArticulation(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr>& vertexes, int pos, 
                    vector<bool> &visited, vector<int> &parent, vector<int> &disc, vector<int> &low) {
    static int time = 0;
    int dfsChild = 0;
    visited[pos] = true;
    disc[pos] = low[pos] = ++time; 

    int a;

    for (int v_pos = 0; v_pos < vertexes.size(); v_pos++) {
      if (context->HelperCheckEdge(vertexes[pos],vertexes[v_pos], ScType(0))) {
        if (!visited[v_pos]) {
          dfsChild++;
          parent[v_pos] = pos;

          if(isArticulation(context, vertexes, v_pos, visited, parent, disc, low))
            return true;

          low[pos] = (low[pos] < low[v_pos]) ? low[pos] : low[v_pos];

          if (parent[pos] == -1 && dfsChild > 1) {
            return true;
          }

          if(parent[pos] != -1 && low[v_pos] >= disc[pos])
            return true;

        } else if (v_pos != parent[pos]) {
          low[pos] = (low[pos] < disc[v_pos]) ? low[pos] : disc[v_pos];
        }
      }
    }                     
    return false;
}


bool isBiconnected(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode) {
    vector<ScAddr> vertexes = getVertexes(context, structNode);

    SC_LOG_DEBUG("graph: " + context->HelperGetSystemIdtf(structNode));	
    SC_LOG_DEBUG("count: " + to_string(vertexes.size()));

    vector<bool> visited(vertexes.size(), false);
    vector<int> parent(vertexes.size(), -1);

    vector<int> disc(vertexes.size());
    vector<int> low(vertexes.size());

    if(vertexes.size() < 3) {
      SC_LOG_COLOR(::utils::ScLog::Type::Debug, "IS NOT BICONNECTED", ScConsole::Color::Magneta);
      return false;
    }
      

    if(isArticulation(context, vertexes, 0, visited, parent, disc, low)) {
      SC_LOG_COLOR(::utils::ScLog::Type::Debug, "IS NOT BICONNECTED", ScConsole::Color::Magneta);
      return false;
    }

    for (auto v : visited) {
      if (!v) {
        SC_LOG_COLOR(::utils::ScLog::Type::Debug, "IS NOT BICONNECTED", ScConsole::Color::Magneta);
        return false;
      }
    }
      
        
    SC_LOG_COLOR(::utils::ScLog::Type::Debug, "IS BICONNECTED", ScConsole::Color::Magneta);
    return true; 
}


SC_AGENT_IMPLEMENTATION(ASearchBiconnectedGraph)
{	
    if (!edgeAddr.IsValid())
      return SC_RESULT_ERROR;

    SC_LOG_DEBUG("INIT");
    
    ScAddr start = ms_context->GetEdgeTarget(edgeAddr);;
    ScAddr graph;

    ScIterator3Ptr iter = ms_context->Iterator3(start, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  
    if (iter->Next()) 
      graph = iter->Get(2);	 	
    else 
      return SC_RESULT_ERROR_INVALID_PARAMS;
    
    generateAnswer(ms_context, start, graph, isBiconnected(ms_context, graph));			

    SC_LOG_DEBUG("DEINIT");
    return SC_RESULT_OK;
}

} // namespace exampleModul