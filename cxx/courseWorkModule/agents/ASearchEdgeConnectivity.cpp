#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include <climits>
#include <algorithm>


#include "ASearchEdgeConnectivity.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{

static int count;

void findMaxFlowUtil(ScMemoryContext* ms_context, ScAddr v1, ScAddr v2, ScAddrVector &visited,
                     ScAddrVector &path, ScAddrVector &vertices, int index) {
  visited.push_back(v1);
  path[index] = v1;
  ++index;

  if (v1 == v2) 
    count++;
  else 
    for (auto &v : vertices) 
      if (!(find(visited.begin(), visited.end(), v) != visited.end())) 
        if(ms_context->HelperCheckEdge(v1, v, ScType(0)) || ms_context->HelperCheckEdge(v, v1, ScType(0))) 
          findMaxFlowUtil(ms_context, v, v2, visited, path, vertices, index);


  --index;
  visited.erase(std::find(visited.begin(), visited.end(), v1));
}


int findMaxFlow(ScMemoryContext* ms_context, ScAddr v1, ScAddr v2, ScAddrVector &vertices) {

  ScAddrVector visited;
  ScAddrVector path(vertices.size());

  int index = 0;
  count = 0;
  
  findMaxFlowUtil(ms_context, v1, v2, visited, path, vertices, index);

  return count;
}


int findEdgeConnectivity(ScMemoryContext* ms_context, ScAddr structNode) {
  ScAddrVector vertices = IteratorUtils::getAllWithType(&(*ms_context), structNode, ScType::NodeConst);
  int ans = INT_MAX;
  int flow;

  for (auto &v1 : vertices) {
    for (auto &v2 : vertices) {
      if (v1 != v2) {
        flow = findMaxFlow(ms_context, v1, v2, vertices);
        ans = min(ans, flow);
      }
      
    }
  }
  return ans;
}


SC_AGENT_IMPLEMENTATION(ASearchEdgeConnectivity)
{	
    if (!edgeAddr.IsValid())
      return SC_RESULT_ERROR;
    
    ScAddr first_node = ms_context->GetEdgeTarget(edgeAddr);;
    ScAddr struct_node;

    ScIterator3Ptr iter = ms_context->Iterator3(first_node, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  
    if (iter->Next()) 
      struct_node = iter->Get(2);	 	
    else 
      return SC_RESULT_ERROR_INVALID_PARAMS;


    int connectivity = findEdgeConnectivity(&(*ms_context), struct_node);


    ScAddr answer = ms_context->CreateNode(ScType::NodeConst);
    ms_context->HelperSetSystemIdtf("connectivity of '" + ms_context->HelperGetSystemIdtf(struct_node) + "' is " + to_string(connectivity), answer);

    SC_LOG_DEBUG("ANSWER: " + to_string(connectivity));

    ScAddrVector answer_stuct = {answer};
    AgentUtils::finishAgentWork(&(*ms_context), first_node, answer_stuct);
    
    return SC_RESULT_OK;
}

} // namespace exampleModul