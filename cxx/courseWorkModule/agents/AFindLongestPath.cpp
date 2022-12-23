#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include <queue>
#include <climits>

#include "AFindLongestPath.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{

using ScAddrSet = set<ScAddr, ScAddLessFunc>;
using ScAddrToIntMap = map<ScAddr, int, ScAddLessFunc>;

ScAddr getGraphAddr(ScMemoryContext* ms_ontext, ScAddr &question) {
  ScIterator3Ptr iter = ms_ontext->Iterator3(question, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  ScAddr graph = question;
  if (iter->Next()) 
    graph = iter->Get(2);	 	
    
  return graph;
}

ScAddr getStartVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  for (auto &v : vertexes) {
    if (ms_context->HelperCheckEdge(Keynodes::rel_start_vertex, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.front();
}

ScAddr getEndVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  for (auto &v : vertexes) {
    if (ms_context->HelperCheckEdge(Keynodes::rel_end_vertex, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.back();
}

bool isElementExist(ScAddrSet &set, ScAddr &element) {
  return (set.find(element) != set.end());
}

bool isEdgeExist(ScMemoryContext* ms_context, ScAddr &v1, ScAddr &v2) {
  return ms_context->HelperCheckEdge(v1, v2, ScType(0));
}

void initWeight(ScAddrToIntMap &weight, ScAddrVector &vertexes, int value) {
  for (auto &v : vertexes) {
    weight.insert(make_pair(v, value));
  }
}

ScAddrToIntMap Dijkstra(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
    ScAddrSet visited;
    ScAddrToIntMap weight;

    initWeight(weight, vertexes, INT_MIN);

    weight[getStartVertex(ms_context, vertexes)] = 0;

    for (auto &v : vertexes) {

      int max_weight = INT_MIN;
      ScAddr max_weight_vertex;

      for (auto &u : vertexes) {
        if (!isElementExist(visited, u) && weight[u] > max_weight) {
          max_weight = weight[u];
          max_weight_vertex = u;
        }
      }

      visited.insert(max_weight_vertex); 

      for (auto &u : vertexes) {
        if (isEdgeExist(ms_context, max_weight_vertex, u) && weight[u] <= weight[max_weight_vertex] ) {
          weight[u] = weight[max_weight_vertex] + 1;
        }
      }
    }
    
    return weight;
}

ScAddrVector findPath(ScMemoryContext* ms_context, ScAddrVector &vertexes, ScAddrToIntMap &weight) {
  ScAddrVector answerSet;

  ScAddr finalVertex = getEndVertex(ms_context, vertexes);
  ScAddr startVertex = getStartVertex(ms_context, vertexes);

  int finalVertexWeight = weight[finalVertex];

  SC_LOG_DEBUG("start: " + ms_context->HelperGetSystemIdtf(startVertex));
  SC_LOG_DEBUG("final: " + ms_context->HelperGetSystemIdtf(finalVertex));

  answerSet.push_back(startVertex);

  while (startVertex != finalVertex) {
    for (auto &u : vertexes) {
      if (isEdgeExist(ms_context, u, finalVertex)) {
        if (finalVertexWeight-1 == weight[u]) {
          answerSet.push_back(finalVertex);
          finalVertexWeight--;
          finalVertex = u;
        }
      }
    }
  }
  return answerSet;
}



void generateAnswer(ScMemoryContext* ms_context, ScAddr &question, ScAddrVector &answer, ScAddr &graph) {
  ScAddr answerNode = ms_context->CreateNode(ScType::NodeConstStruct);
  ms_context->HelperSetSystemIdtf(
    "максимальный путь между '" + 
    ms_context->HelperGetSystemIdtf(answer.front()) + 
    "' и '" + 
    ms_context->HelperGetSystemIdtf(answer.back()) + "' для теста '" +
    ms_context->HelperGetSystemIdtf(graph) + "'",
    answerNode);

  ScAddr before = answer.front();
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, before);

  ScAddrVector::iterator iter = answer.end();
  for (auto iter = answer.end()-1; iter != answer.begin(); --iter) {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeDCommonConst, before, *iter);

    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, *iter);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, edge);

    before = *iter;
  }

  AgentUtils::finishAgentWork(ms_context, question, (ScAddrVector){answerNode}, true);
}

SC_AGENT_IMPLEMENTATION(AFindLongestPath)
{	
    SC_LOG_DEBUG("[START]");
    
    ScAddr start = ms_context->GetEdgeTarget(edgeAddr);;
    ScAddr graph = getGraphAddr(ms_context.get(), start);

    ScAddrVector vertexes = IteratorUtils::getAllWithType(ms_context.get(), graph, ScType::NodeConst);
    ScAddrToIntMap weight = Dijkstra(ms_context.get(), vertexes);
   
    for (auto &w: weight) {
      SC_LOG_DEBUG("w: " + ms_context->HelperGetSystemIdtf(w.first) + ": " + to_string(w.second));
    }

    ScAddrVector answer = findPath(ms_context.get(), vertexes, weight);

    SC_LOG_DEBUG("PATH:");
    for (auto &v : answer) {
      SC_LOG_DEBUG("p: " + ms_context->HelperGetSystemIdtf(v));
    }

    generateAnswer(ms_context.get(), start, answer, graph);

    SC_LOG_DEBUG("[END]");
    return SC_RESULT_OK;
}

}