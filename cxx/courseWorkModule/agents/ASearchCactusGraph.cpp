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


#include "ASearchCactusGraph.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{

int f;

struct Edge {
  int to;
  int incycle;

  Edge(int to, int incycle) : to(to), incycle(incycle) {}
};

vector<vector<Edge> > EdgeList;
vector<int> used;
vector<Edge *> in;

void dfs(int vertex, int prev) {
  used[vertex] = 1;

  for (int a = 0; a < EdgeList[vertex].size(); a++) {
    Edge &edge = EdgeList[vertex][a];
    int to = edge.to;

    if (to == prev) {
      continue;
    } 

    if (used[to] == 0) {
      in.push_back(&edge);
      dfs(to, vertex);
      in.pop_back();
    } 
    
    else {
      if (used[to] == 1) {
        f &= !edge.incycle;
        edge.incycle = 1;
        int ind = in.size() - 1;
        
        while (in[ind]->to != edge.to & ind > 0) {
          f &= !in[ind]->incycle;
          in[ind]->incycle = 1;
          ind--;
        }
      }
      if (!f) return;
    }
  }
  used[vertex] = 2;
}

void printEdgeList(ScMemoryContext* ms_context, ScAddrVector vertexes) {
  SC_LOG_DEBUG("EdgeList:");
  
  for (int i = 0; i < EdgeList.size(); i++) {
    string s;
    s = ms_context->HelperGetSystemIdtf( vertexes[i]) + "-->";
    for (int a = 0; a < EdgeList[i].size(); a++) {
      s += ms_context->HelperGetSystemIdtf( vertexes[EdgeList[i][a].to] ) + ", ";
    }
    SC_LOG_DEBUG(s);
  }
}

int getNumber(ScAddrVector vertexes, ScAddr v) {
  int counter = -1;
  for (auto &i: vertexes) {
    counter++;
    if (i == v)
      break;
  }
  return counter;
}

void addEdge(ScAddrVector vertexes, int v1, int v2) {
  EdgeList[getNumber(vertexes, vertexes[v1])].push_back(Edge(v2, 0));
  
}

bool isGraphACactus(ScMemoryContext* ms_context, ScAddr struct_node) {
  ScAddrVector vertexes = IteratorUtils::getAllWithType(ms_context, struct_node, ScType::NodeConst);
    ScAddrVector edges = IteratorUtils::getAllWithType(ms_context, struct_node, ScType::EdgeUCommonConst);

    SC_LOG_DEBUG("vertexes:" + to_string(vertexes.size()));
    SC_LOG_DEBUG("edges:" + to_string(edges.size()));
    

    EdgeList.clear();
    used.clear();
    in.clear();

    EdgeList.resize(vertexes.size());
    used.resize(vertexes.size());
   
    for (int v1 = 0; v1 < vertexes.size(); v1++) {
      for (int v2 = 0; v2 < vertexes.size(); v2++) { 
        if (ms_context->HelperCheckEdge(vertexes[v1], vertexes[v2], ScType(0)) || ms_context->HelperCheckEdge(vertexes[v2], vertexes[v1], ScType(0))) {
          addEdge(vertexes, v1, v2);
          // EdgeList[v2].push_back(Edge(v1, 0));
          SC_LOG_DEBUG(ms_context->HelperGetSystemIdtf(vertexes[v1]) + " : " + ms_context->HelperGetSystemIdtf(vertexes[v2]));
        }
      }
    }

    SC_LOG_DEBUG("vertexes size: " + to_string(vertexes.size()));
    SC_LOG_DEBUG("edges size: " + to_string(EdgeList.size()));
    

    printEdgeList(&(*ms_context), vertexes);

    SC_LOG_DEBUG("============");

    f = 1;
    dfs(getNumber(vertexes, vertexes[0]), -1);

    SC_LOG_DEBUG("after dfs");
    if (f) {
      for (int a = 0; a < EdgeList.size(); a++) {
        f &= (used[a] == 2);
      }
    }

    if (f) {
      SC_LOG_DEBUG("is cactus");  
      return true;
    } else {
      SC_LOG_DEBUG("is not cactus");
      return false;
    }
}

SC_AGENT_IMPLEMENTATION(ASearchCactusGraph)
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


    ScAddr output = ms_context->CreateNode(ScType::NodeConst);
    if(isGraphACactus(&(*ms_context), struct_node)) {
      string label = "'" + ms_context->HelperGetSystemIdtf(struct_node) + "' is cactus";
      ms_context->HelperSetSystemIdtf(label, output);
    } else {
      string label = "'" + ms_context->HelperGetSystemIdtf(struct_node) + "' is NOT cactus";
      ms_context->HelperSetSystemIdtf(label, output);
    }

    ScAddrVector answer = {output};
    AgentUtils::finishAgentWork(&(*ms_context), first_node, answer);
    
    return SC_RESULT_OK;
}

} // namespace exampleModul