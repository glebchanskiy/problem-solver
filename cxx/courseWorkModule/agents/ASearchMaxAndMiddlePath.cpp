#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include <queue>
#include <climits>

#include "ASearchMaxAndMiddlePath.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{
using ScAddrSet = set<ScAddr, ScAddLessFunc>;

bool findElement(ScAddrSet &visited, ScAddr &element) {
  return visited.find(element) != visited.end();
}

bool checkUCommonEdge(ScMemoryContext* ms_context, ScAddr v1, ScAddr v2) {
  return ms_context->HelperCheckEdge(v1, v2, ScType::EdgeUCommonConst) || ms_context->HelperCheckEdge(v2, v1, ScType::EdgeUCommonConst); 
}

ScAddr getFirstCentralVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  for (auto &v : vertexes) {
    if (ms_context->HelperCheckEdge(Keynodes::rel_central_1, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.front();
}

ScAddr getSecondCentralVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  for (auto &v : vertexes) {
    if (ms_context->HelperCheckEdge(Keynodes::rel_central_2, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.back();
}

void print(ScMemoryContext* ms_context, vector<ScAddrVector> paths) {
  SC_LOG_DEBUG("count: " + to_string(paths.size()));
  SC_LOG_DEBUG("PATHS:");
  for (auto &path : paths) {
    string msg;
    for (auto &v : path) {
      if (v != path.back()) 
        msg += ms_context->HelperGetSystemIdtf(v) + " <-> ";
      else
        msg += ms_context->HelperGetSystemIdtf(v);
    }
    SC_LOG_DEBUG(msg); 
  }
}

int getMaxPath(vector<ScAddrVector> paths) {
  int maxPath = paths.front().size();

  for (auto &path : paths) {
    if (maxPath < path.size()) {
      maxPath = path.size();
    }
  }
  SC_LOG_DEBUG("max path: " + to_string(maxPath-1));
  return maxPath - 1; // вес - количество ребер, те кол-во вершин отнять 1
}

double getMidPath(vector<ScAddrVector> paths) {
  int sumWeigth = 0;
  int count = paths.size();

  for (auto &path : paths) {
    sumWeigth += path.size() - 1;
  }

  double midPath = (double) sumWeigth / (double) count;
  SC_LOG_DEBUG("mid path: " + to_string(midPath));
  return midPath;
}

void dfs(ScMemoryContext* ms_context, ScAddrSet &visited, ScAddrVector &vertexes, vector<ScAddrVector> &paths, ScAddr &currentVertex, ScAddr &endVertex, ScAddrVector &path) {
  
  if (currentVertex == endVertex) {
    paths.push_back(path);
    return;
  }

  visited.insert(currentVertex);

  for (auto &vertex : vertexes) {
    if (!findElement(visited, vertex)) {
      if (checkUCommonEdge(ms_context, currentVertex, vertex)) {
        path.push_back(vertex);
        dfs(ms_context, visited, vertexes, paths, vertex, endVertex, path);
        path.erase(remove(path.begin(), path.end(), vertex));
        
      }
    }
  }
  visited.erase(currentVertex);
}

void findPaths(ScMemoryContext* ms_context, ScAddr graph, ScAddr quest) {

  ScAddrSet visited = {};
  ScAddrVector vertexes = IteratorUtils::getAllWithType(ms_context, graph, ScType::NodeConst);

  ScAddr start = getFirstCentralVertex(ms_context, vertexes);
  ScAddr end = getSecondCentralVertex(ms_context, vertexes);

  ScAddrVector buff_path = {start};
  vector<ScAddrVector> paths;

  dfs(ms_context, visited, vertexes, paths, start, end, buff_path);

  print(ms_context, paths);

  int maxPath = getMaxPath(paths);
  double midPath = getMidPath(paths);

  ScAddr max = ms_context->CreateNode(ScType::NodeConst);
  ScAddr mid = ms_context->CreateNode(ScType::NodeConst);

  ms_context->HelperSetSystemIdtf("максимальный путь между центарльными вершинами '" +
     ms_context->HelperGetSystemIdtf(start) + 
     "' и '" +
     ms_context->HelperGetSystemIdtf(end) + 
     "' в тесте '" +
     ms_context->HelperGetSystemIdtf(quest) +
     "' равен " + 
     to_string(maxPath), 
     max
  );

  ms_context->HelperSetSystemIdtf("средний путь между центарльными вершинами '" +
     ms_context->HelperGetSystemIdtf(start) + 
     "' и '" +
     ms_context->HelperGetSystemIdtf(end) + 
     "' в тесте '" +
     ms_context->HelperGetSystemIdtf(quest) +
     "' равен " + 
     to_string(midPath), 
     mid
  );

  ScAddrVector answer = {max, mid};
  AgentUtils::finishAgentWork(ms_context, quest, answer);

}

SC_AGENT_IMPLEMENTATION(ASearchMaxAndMiddlePath)
{	
    ScAddr quest = ms_context->GetEdgeTarget(edgeAddr);;

    ScIterator3Ptr iter = ms_context->Iterator3(quest, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    ScAddr graph;
    if (iter->Next()) 
      graph = iter->Get(2);	 

    findPaths(ms_context.get(), graph, quest);

    return SC_RESULT_OK;
}

}