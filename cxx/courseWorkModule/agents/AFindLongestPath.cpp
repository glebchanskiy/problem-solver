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
// обычные дефайны, чтобы не писать кучу букав
using ScAddrSet = set<ScAddr, ScAddLessFunc>;
using ScAddrToIntMap = map<ScAddr, int, ScAddLessFunc>;
// vector<ScAddr> в остисе имеет дефайн как ScAddrVector
// я решил придерживаться стиля и сделал ScAddrSet и ScAddrToIntMap
//
// мапа (ScAddrToIntMap) нужна для хранения веса каждой вершины (от начальной)
// в ней ключ - это адрес, а значение вес.
// именно мапу использовал, потому что удобно, map[адрес] = значение
// гораздо проще чем думать в каком порядке идут индексы и тп 
//
// сет использовал для посещенных вершин т.к сет - это уникальная коллекция, опять же легко взаимодействовать 
// (если адрес есть в сете, значит поситили, нету - ну значит нет) легко добавлять и удалять адреса


// получаем адрес графа
ScAddr getGraphAddr(ScMemoryContext* ms_ontext, ScAddr &question) {
  // Iterator(начальная вершина, по каким дугам, в какие вершины)
  ScIterator3Ptr iter = ms_ontext->Iterator3(question, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  ScAddr graph = question; 

  if (iter->Next()) 
    // Get(0) - начальная вершина
    // Get(1) - ребро
    // Get(2) - конечная вершина
    graph = iter->Get(2); 	
    
  return graph; 
}

// получаем вершину с которой ищем пути
ScAddr getStartVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  // проходим по всем вершинам
  for (auto &v : vertexes) {
    // и если находим такую, что есть ребро между нашей вершиной и rel_start_vertex, (мы её описываем в папку keynodes)
    //(мы так в тесах помечаем начальную вершину) то возвращаем её адерс.
    if (ms_context->HelperCheckEdge(Keynodes::rel_start_vertex, v, ScType::EdgeAccessConstPosPerm)) // HelperCheckEdge(начальная вершина, конечная, тип ребра)
      return v;
  }
  return vertexes.front(); // если не нашли, берём первую
}

// тоже самое, что и getStartVertex, только тут конечная
ScAddr getEndVertex(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
  for (auto &v : vertexes) {
    if (ms_context->HelperCheckEdge(Keynodes::rel_end_vertex, v, ScType::EdgeAccessConstPosPerm))
      return v;
  }
  return vertexes.back();
}

// проверяем есть ли в сете наш элемент, 
bool isElementExist(ScAddrSet &set, ScAddr &element) {
  return (set.find(element) != set.end());
}

// вынес проверку существования ребра
bool isEdgeExist(ScMemoryContext* ms_context, ScAddr &v1, ScAddr &v2) {
  return ms_context->HelperCheckEdge(v1, v2, ScType(0)); // ScType(0) - любой тип, нам нужен DCommon, но так проще
}

// инитим мапу ключами и значениями весов 
void initWeight(ScAddrToIntMap &weight, ScAddrVector &vertexes, int value) {
  // создаём ключи в мапе для кажой вершины 
  for (auto &v : vertexes) {
    weight.insert(make_pair(v, value)); // в мапу инсертится пара (ключ - значение)
  }
}

// обычный дейкстра (не рекурсивынй только), только переделанный под SC память
ScAddrToIntMap Dijkstra(ScMemoryContext* ms_context, ScAddrVector &vertexes) {
    ScAddrSet visited;
    ScAddrToIntMap weight;

    initWeight(weight, vertexes, INT_MIN); // инитим мапу минимальным возможным интом

    weight[getStartVertex(ms_context, vertexes)] = 0; // начальная вершина вес - 0

    for (auto &v : vertexes) { 

      int max_weight = INT_MIN; 
      ScAddr max_weight_vertex; // тут будет вдрес вершины с наибольшим весом

      for (auto &u : vertexes) { // для кажой вершины
        // если она не vis и вес её больше макс найденного
        if (!isElementExist(visited, u) && weight[u] > max_weight) {
          max_weight = weight[u]; // обновляем максы
          max_weight_vertex = u;  // запоминаем адрес
        }
      }

      // после цикла у нас есть адрес самой тяжёлой вершины

      visited.insert(max_weight_vertex); // мы её посетили 

      // обновляем веса у соседей 
      for (auto &u : vertexes) {
        if (isEdgeExist(ms_context, max_weight_vertex, u) && weight[u] <= weight[max_weight_vertex] ) {
          weight[u] = weight[max_weight_vertex] + 1;
        }
      }
    }
    
    return weight; // возвращаем мапу
}

// на основе мапы весов вершин сторим путь
ScAddrVector findPath(ScMemoryContext* ms_context, ScAddrVector &vertexes, ScAddrToIntMap &weight) {
  ScAddrVector answerSet; // тут будет лежать путь

  ScAddr startVertex = getStartVertex(ms_context, vertexes);
  ScAddr finalVertex = getEndVertex(ms_context, vertexes);

  int finalVertexWeight = weight[finalVertex];

  SC_LOG_DEBUG("start: " + ms_context->HelperGetSystemIdtf(startVertex)); // выводим идентфктры вершин
  SC_LOG_DEBUG("final: " + ms_context->HelperGetSystemIdtf(finalVertex));

  answerSet.push_back(startVertex);

  // тут мы строим путь опираясь на макс веса
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


// создаём ответ в остисе
void generateAnswer(ScMemoryContext* ms_context, ScAddr &question, ScAddrVector &answer, ScAddr &graph) {
  ScAddr answerNode = ms_context->CreateNode(ScType::NodeConstStruct);
  // это будет нода (структурка) содержащая наш путь 

  // обзываем её так:
  ms_context->HelperSetSystemIdtf(
    "максимальный путь между '" + 
    ms_context->HelperGetSystemIdtf(answer.front()) + 
    "' и '" + 
    ms_context->HelperGetSystemIdtf(answer.back()) + "' для теста '" +
    ms_context->HelperGetSystemIdtf(graph) + "'",
    answerNode);

  // тут танцы с бубном ради нормального отображения пути
  // не знаю как его правильно можно добвить в контур
  // я пока делаю так:
  // создаю ноду ответа (та что выше)
  // создаю между вершинами пути рёбра и цепляю их к ответу (и вершины и рёбра)
  ScAddr before = answer.front();
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, before);

  ScAddrVector::iterator iter = answer.end();
  for (auto iter = answer.end()-1; iter != answer.begin(); --iter) {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeDCommonConst, before, *iter);

    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, *iter);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answerNode, edge);

    before = *iter;
  }

  // это штука просто огонь, недавно узнал, так вот просто передаёшь вектор нод в неё и оно всё сделает.
  AgentUtils::finishAgentWork(ms_context, question, (ScAddrVector){answerNode}, true);
}

SC_AGENT_IMPLEMENTATION(AFindLongestPath)
{	
    SC_LOG_DEBUG("[START]");
    
    // получаем адрес пустой ноды
    ScAddr start = ms_context->GetEdgeTarget(edgeAddr);;
    // получаем адрес графа
    ScAddr graph = getGraphAddr(ms_context.get(), start);

    // получаем все вершины типа Нода (наши вершины графа)
    ScAddrVector vertexes = IteratorUtils::getAllWithType(ms_context.get(), graph, ScType::NodeConst);
    // получаем наши макс веса
    ScAddrToIntMap weight = Dijkstra(ms_context.get(), vertexes);

    // выводим веса
    for (auto &w: weight) {
      SC_LOG_DEBUG("w: " + ms_context->HelperGetSystemIdtf(w.first) + ": " + to_string(w.second));
    }

    // получаем путь как вектор
    ScAddrVector answer = findPath(ms_context.get(), vertexes, weight);

    // выводим путь
    SC_LOG_DEBUG("PATH:");
    for (auto &v : answer) {
      SC_LOG_DEBUG("p: " + ms_context->HelperGetSystemIdtf(v));
    }

    // создаём путь в памяти
    generateAnswer(ms_context.get(), start, answer, graph);

    SC_LOG_DEBUG("[END]");
    return SC_RESULT_OK;
}

}