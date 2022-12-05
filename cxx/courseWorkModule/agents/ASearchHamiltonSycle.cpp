#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_template_search.cpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include "ASearchHamiltonSycle.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{

const bool DEBUG_ON = false;


void debug(string msg);

void viewPath(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr> &path);

vector<ScAddr> getAllVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode);

bool isSafe(const std::unique_ptr<ScMemoryContext>& context, ScAddr vertex, int currentPosition, vector<ScAddr> &path);

bool hamilton(const std::unique_ptr<ScMemoryContext>& context, int currentPosition, vector<ScAddr> &path, const vector<ScAddr> &allVertexes);


SC_AGENT_IMPLEMENTATION(ASearchHamiltonSycle)
{	
    if (!edgeAddr.IsValid())
      return SC_RESULT_ERROR;

    SC_LOG_DEBUG("INIT GamAgent");
    
				// получаем ноду структуры (графа)
				ScAddr startNode = ms_context->GetEdgeTarget(edgeAddr);
				ScAddr structNode;

				ScIterator3Ptr iter = ms_context->Iterator3(startNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

				if (iter->Next())
        structNode = iter->Get(2);
				else {
					 SC_LOG_DEBUG("INVALID PARAMS");	
					 return SC_RESULT_ERROR_INVALID_PARAMS;			
				}
        

				// получаем все верщины графа
				vector<ScAddr> allVertexes = getAllVertexes(ms_context, structNode);
				// путь в графе (если есть)
				vector<ScAddr> path(allVertexes.size(), allVertexes[0]);


				debug("first vertex: " + ms_context->HelperGetSystemIdtf(allVertexes[0]));


				SC_LOG_DEBUG("graph name: " + ms_context->HelperGetSystemIdtf(structNode));	
				SC_LOG_DEBUG("verteces count: " + to_string(allVertexes.size()));

				//// генерируем ответ
    // создаём ноду ответа как структуру
    ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
				ms_context->HelperSetSystemIdtf("hamilton cycle for '" + ms_context->HelperGetSystemIdtf(structNode) + "'", answer);
				
				ScAddr arc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, startNode, answer);
				ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_answer, arc);


				if (hamilton(ms_context, 1, path, allVertexes)) {
					  // если цикл существует
							SC_LOG_COLOR(::utils::ScLog::Type::Debug, "CYCLE EXIST", ScConsole::Color::Magneta);

							viewPath(ms_context, path);
		     // добавляем вершины в ответ
							for (auto &v : path) {
					  		ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, v);
							}

				} else {
					  SC_LOG_COLOR(::utils::ScLog::Type::Debug, "CYCLE NOT EXIST", ScConsole::Color::Red);
							// если цикл не существует, то структура остаётся пустой
				}

				SC_LOG_DEBUG("DEINIT GamAgent");
    return SC_RESULT_OK;
}


bool hamilton(const std::unique_ptr<ScMemoryContext>& context, int currentPosition, vector<ScAddr> &path, const vector<ScAddr> &allVertexes) {
		debug("hamilton:");
		if (currentPosition == allVertexes.size()) {
			 debug("hamilton: pos == size; pos=" + to_string(currentPosition) + "; size=" + to_string(allVertexes.size()));
				debug("hamilron: l_p = " + context->HelperGetSystemIdtf(path[currentPosition-1]) + ", f_p = " + context->HelperGetSystemIdtf(path[0]));

				debug("hamilron: edge exists? = " + to_string(context->HelperCheckEdge(path[currentPosition-1], path[0], ScType(0))));
				return context->HelperCheckEdge(path[currentPosition-1], path[0], ScType(0)); // -1
		}
		debug("hamilton: pos != size; pos=" + to_string(currentPosition) + "; size=" + to_string(allVertexes.size()));

		 for (auto &vertex : allVertexes) {
			debug("hamilton: for-vertx: " + context->HelperGetSystemIdtf(vertex));

				if (isSafe(context, vertex, currentPosition, path)) {
					 path[currentPosition] = vertex;
						
						if (hamilton(context, currentPosition + 1, path, allVertexes)) {
							 debug("hamilton: return true");
								return true;
						}
						debug("hamilton: return false");

						path[currentPosition] = allVertexes[0];
								
				}
		}

  debug("hamilton: after loop");
		return false;
}


bool isSafe(const std::unique_ptr<ScMemoryContext>& context, ScAddr vertex, int currentPosition, vector<ScAddr> &path) {
		if (!context->HelperCheckEdge(path[currentPosition-1], vertex, ScType(0))) { // -1
			 debug("hamilton -> isSafe: edge not exist");
				return false;
		}

		for (auto &v : path) {
			 if (v == vertex) {
					debug("hamilton -> isSafe: vertex already in path");
					return false;
				} 
		}
		debug("hamilton -> isSafe: true");
		return true;
		
}


vector<ScAddr> getAllVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode) {
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


void viewPath(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr> &path) {
				string pathAsString;
				for (auto &i : path) {
					 if (i == path.back())
								pathAsString += context->HelperGetSystemIdtf(i);
						else
						  pathAsString += context->HelperGetSystemIdtf(i) + " -> ";
				}
				SC_LOG_DEBUG("path: " + pathAsString);
}


void debug(string msg) {
	 if (DEBUG_ON) {
			 SC_LOG_DEBUG(msg);
		}
}


} // namespace exampleModul