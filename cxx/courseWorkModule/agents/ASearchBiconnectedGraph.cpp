#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>


#include "ASearchBiconnectedGraph.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{
  
static int counter = 0;

bool isArticulationExist(ScMemoryContext *ms_context, vector<ScAddr>& vertexes, int current, 
                    vector<bool> &v, vector<int> &p, vector<int> &d, vector<int> &l) {
    v[current] = true;
    int dfsChild = 0;
    d[current] = l[current] = ++counter; 

    for (int v_current = 0; v_current < vertexes.size(); v_current++) {
      if (ms_context->HelperCheckEdge(vertexes[current],vertexes[v_current], ScType(0)) || ms_context->HelperCheckEdge(vertexes[v_current],vertexes[current], ScType(0))) {
        if (!v[v_current]) {
          dfsChild++;
          p[v_current] = current;

          if(isArticulationExist(ms_context, vertexes, v_current, v, p, d, l))
            return true;

          l[current] = (l[current] < l[v_current]) ? l[current] : l[v_current];

          if (p[current] == -1 && dfsChild > 1) {
            return true;
          }
          if(p[current] != -1 && l[v_current] >= d[current])
            return true;
        } else if (v_current != p[current]) {
          l[current] = (l[current] < d[v_current]) ? l[current] : d[v_current];
        }
      }
    }                     
    return false;
}


bool isBiconnected(ScMemoryContext *ms_context, ScAddr graph) {
    vector<ScAddr> vertexes = IteratorUtils::getAllWithType(&(*ms_context), graph, ScType::NodeConst);

    vector<bool> v(vertexes.size(), false);
    vector<int> p(vertexes.size(), -1);
    vector<int> l(vertexes.size());
    vector<int> d(vertexes.size());
    
    if(vertexes.size() <= 2 || isArticulationExist(ms_context, vertexes, 0, v, p, d, l)) {
      return false;
    }

    for (auto i : v) {
      if (!i) {
        return false;
      }
    }
        
    return true; 
}


SC_AGENT_IMPLEMENTATION(ASearchBiconnectedGraph)
{	
    if (!edgeAddr.IsValid())
      return SC_RESULT_ERROR;

    ScAddr question = ms_context->GetEdgeTarget(edgeAddr);;
    ScAddr structure;

    ScIterator3Ptr iter = ms_context->Iterator3(question, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  
    if (iter->Next()) 
      structure = iter->Get(2);	 	
    else 
      return SC_RESULT_ERROR_INVALID_PARAMS;

    ScAddr node = ms_context->CreateNode(ScType::NodeConst);
    string graphIdtf = ms_context->HelperGetSystemIdtf(structure);

    if(isBiconnected(&(*ms_context), structure)) {
      ms_context->HelperSetSystemIdtf("graph '" + graphIdtf + "' is biconnected", node);
      SC_LOG_COLOR(ScLog::Type::Debug, "graph '" + graphIdtf + "' is biconnected", ScConsole::Color::Blue);
    } else {
      ms_context->HelperSetSystemIdtf("graph '" + graphIdtf + "' is not biconnected", node); 
      SC_LOG_COLOR(ScLog::Type::Debug, "graph '" + graphIdtf + "' is not biconnected", ScConsole::Color::Red);
    }

    vector<ScAddr> answer = {node};
    AgentUtils::finishAgentWork(&(*ms_context), question, answer, true);
    
    return SC_RESULT_OK;
}

} // namespace exampleModul