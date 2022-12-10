#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include <queue>

#include "ASearchPathInGraph.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{

void viewPath(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr> &path);

vector<ScAddr> getAllVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode);


ScAddr getFirstVertex(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr> &vertexes) {
  for (auto &v : vertexes) {
    if (context->HelperCheckEdge(Keynodes::rel_first_vertex, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.front();
}


int getNumber(ScAddr vertex, vector<ScAddr> vertexes) {
  for (int i = 0; i < vertexes.size(); i++) {
    if (vertexes[i] == vertex)
      return i;
  }
  return 0;
}


void setMinDist(vector<pair<ScAddr, int>> &min_dist, ScAddr v, int value) {
  for (auto &i : min_dist) {
    if (i.first == v) {
      if (i.second > value)
        i.second = value;
    }
  }
}


vector<pair<ScAddr, int>> findPath(const std::unique_ptr<ScMemoryContext>& context, ScAddr graph) {


    vector<ScAddr> vertexes = getAllVertexes(context, graph);

    vector<int> distance(vertexes.size(), INT_MAX);
    vector<bool> visited(vertexes.size(), false);

    vector<pair<ScAddr, int>> min_dist(vertexes.size());


    ScAddr first = getFirstVertex(context, vertexes);
    int firstVertexNumber = getNumber(first, vertexes);


    for (int i = 0; i<vertexes.size(); i++) {
        min_dist[i].first = vertexes[i];
        min_dist[i].second = INT_MAX;
    }
    setMinDist(min_dist, first, 0);

    SC_LOG_DEBUG("graph name   : " + context->HelperGetSystemIdtf(graph)); 
    SC_LOG_DEBUG("vertex count : " + to_string(vertexes.size())); 
    SC_LOG_DEBUG("first vetex  : " + context->HelperGetSystemIdtf(first)); 
    

    distance[firstVertexNumber] = 0;
    visited[firstVertexNumber] = true;

    queue<int> vertexesQueue;
    vertexesQueue.push(firstVertexNumber);

    while (!vertexesQueue.empty()) {
      int vertex = vertexesQueue.front();
      vertexesQueue.pop();
      visited[vertex] = true;

      for (auto &v : vertexes) {
        int v_num = getNumber(v, vertexes);
        if (context->HelperCheckEdge(v, vertexes[vertex], ScType(0)) && !visited[v_num]) {
          vertexesQueue.push(v_num);
          distance[v_num] = distance[vertex] + 1;
          
          setMinDist(min_dist, v, distance[v_num]);
        }
      }
    }

    SC_LOG_DEBUG("MIN PATHS:"); 

    for (auto &i : min_dist) {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(i.first) + " = " + to_string(i.second)); 
    }

    return min_dist;
}


void recursiveMakeAnswerTree(const std::unique_ptr<ScMemoryContext>& context, vector<pair<ScAddr, int>> &min_dist, ScAddr node, int n, ScAddr answer) {
  for (auto &v : min_dist) {  
    if (v.second == n) {
      ScAddr edge = context->CreateEdge(ScType::EdgeDCommonConst, node, v.first);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, v.first);
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, edge);

      SC_LOG_DEBUG("a : " + context->HelperGetSystemIdtf(node) + " -> " + context->HelperGetSystemIdtf(v.first) + "   " + to_string(n));
      recursiveMakeAnswerTree(context, min_dist, v.first, n+1, answer);
    }
  }
}


void answer(const std::unique_ptr<ScMemoryContext>& context, vector<pair<ScAddr, int>> &min_dist, ScAddr start, ScAddr graph) {
    ScAddr answer = context->CreateNode(ScType::NodeConstStruct);
		context->HelperSetSystemIdtf("paths for '" + context->HelperGetSystemIdtf(graph) + "'", answer);

    ScAddr edge = context->CreateEdge(ScType::EdgeDCommonConst, start, answer);
		context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_answer, edge);

    recursiveMakeAnswerTree(context, min_dist, answer, 0, answer);
}


ScAddr findGraphAddr(const std::unique_ptr<ScMemoryContext>& context, ScAddr start) {
   ScIterator3Ptr iter = context->Iterator3(start, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  
    if (iter->Next()) 
      start = iter->Get(2);	 	
    
    return start;
}


SC_AGENT_IMPLEMENTATION(ASearchPathInGraph)
{	
    SC_LOG_DEBUG("start");
    
    // get start node
    ScAddr start = ms_context->GetEdgeTarget(edgeAddr);;
    // get graph stucture node
    ScAddr graph = findGraphAddr(ms_context, start);

    // vertex and distance to it
    vector<pair<ScAddr, int>> min_dist = findPath(ms_context, graph);

    // make answer tree
    answer(ms_context, min_dist, start, graph);

    SC_LOG_DEBUG("destroy");
    return SC_RESULT_OK;
}

} // namespace exampleModul