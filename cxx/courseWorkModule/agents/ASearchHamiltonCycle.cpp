#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_template_search.cpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/SetOperationsUtils.hpp>

#include "ASearchHamiltonCycle.hpp"

using namespace std;
using namespace utils;

namespace courseWorkNamespace
{


void debug(string msg);

void viewPath(const std::unique_ptr<ScMemoryContext>& context, vector<ScAddr> &path);

vector<ScAddr> getAllVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode);

bool isSafe(const std::unique_ptr<ScMemoryContext>& context, ScAddr vertex, int currentPosition, vector<ScAddr> &path);

bool hamilton(const std::unique_ptr<ScMemoryContext>& context, int currentPosition, vector<ScAddr> &path, const vector<ScAddr> &allVertexes);

void deleteAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr answer);

ScAddr createAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode, ScAddr startNode, vector<ScAddr> &path);

ScAddr createEmptyAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode, ScAddr startNode);

void addToAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr answer, vector<ScAddr> &path);

ScAddr getIfExist(const std::unique_ptr<ScMemoryContext>& context, ScAddr startNode);

vector<ScAddr> findHamiltonCycle(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode);



SC_AGENT_IMPLEMENTATION(ASearchHamiltonCycle)
{	
    if (!edgeAddr.IsValid())
      return SC_RESULT_ERROR;

    SC_LOG_DEBUG("INIT [ASearchHamiltonCycle]");
    
				ScAddr startNode, structNode;

				try {
					 startNode = ms_context->GetEdgeTarget(edgeAddr);
						structNode = getIfExist(ms_context, startNode);
				} catch (ExceptionInvalidParams e) {
						SC_LOG_DEBUG(e.Message());	
						return SC_RESULT_ERROR;
				}

				vector<ScAddr> path = findHamiltonCycle(ms_context, structNode);

				if (!path.empty()) {
							createAnswer(ms_context, structNode, startNode, path);
				} else {
							createEmptyAnswer(ms_context, structNode, startNode);
				}

				SC_LOG_DEBUG("DEINIT [ASearchHamiltonCycle]");
    return SC_RESULT_OK;
}


vector<ScAddr> findHamiltonCycle(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode) {
				vector<ScAddr> allVertexes = getAllVertexes(context, structNode);
				vector<ScAddr> path(allVertexes.size(), allVertexes[0]);

				SC_LOG_DEBUG("graph name: " + context->HelperGetSystemIdtf(structNode));	
				SC_LOG_DEBUG("verteces count: " + to_string(allVertexes.size()));

				if (hamilton(context, 1, path, allVertexes) && (allVertexes.size() > 2)) {
							SC_LOG_COLOR(::utils::ScLog::Type::Debug, "CYCLE EXIST", ScConsole::Color::Magneta);
							viewPath(context, path);
				} else {
					  SC_LOG_COLOR(::utils::ScLog::Type::Debug, "CYCLE NOT EXIST", ScConsole::Color::Red);
							path.clear();
				}

				return path;
}


bool hamilton(const std::unique_ptr<ScMemoryContext>& context, int currentPosition, vector<ScAddr> &path, const vector<ScAddr> &allVertexes) {

		if (currentPosition == allVertexes.size()) {
				return context->HelperCheckEdge(path[currentPosition-1], path[0], ScType(0)); // -1
		}
		
		for (auto &vertex : allVertexes) {
				if (isSafe(context, vertex, currentPosition, path)) {
					 path[currentPosition] = vertex;
						if (hamilton(context, currentPosition + 1, path, allVertexes)) {							 
								return true;
						}					
						path[currentPosition] = allVertexes[0];								
				}
		}
		return false;
}


bool isSafe(const std::unique_ptr<ScMemoryContext>& context, ScAddr vertex, int currentPosition, vector<ScAddr> &path) {
		if (!context->HelperCheckEdge(path[currentPosition-1], vertex, ScType(0))) { // -1
				return false;
		}
		for (auto &v : path) {
			 if (v == vertex) {
					return false;
				} 
		}
		return true;
		
}


ScAddr getIfExist(const std::unique_ptr<ScMemoryContext>& context, ScAddr startNode) {
		 ScIterator3Ptr iter = context->Iterator3(startNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
			ScAddr structNode;

			if (iter->Next()) 
   		structNode = iter->Get(2);	 	
			else 
					SC_THROW_EXCEPTION(ExceptionInvalidParams, "INVALID PARAMS (structure not exists)");
			
			if (context->GetElementType(structNode) != ScType::NodeConstStruct) 		
					SC_THROW_EXCEPTION(ExceptionInvalidParams, "INVALID PARAMS (wrong structure type)");

			return structNode;
}





vector<ScAddr> getAllVertexes(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode) {
				vector<ScAddr> allVertexes;
				ScIterator3Ptr it = context->Iterator3(structNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

				while (it->Next()) {
							ScAddr node = it->Get(2);
							ScType node_type = context->GetElementType(node);

							if (node_type == ScType::NodeConst) {
								 SC_LOG_DEBUG("added: " + context->HelperGetSystemIdtf(node));
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


void deleteAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr answer) {
		ScIterator3Ptr it = context->Iterator3(answer, ScType::EdgeAccessConstPosPerm, ScType(0));
				while (it->Next()) {
							ScAddr node = it->Get(2);
							ScType nodeType = context->GetElementType(node);
							if (nodeType == ScType::EdgeDCommonConst) {
								 context->EraseElement(node);
							}			
				}
}


ScAddr createEmptyAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode, ScAddr startNode) {
	  	ScAddr answer = context->CreateNode(ScType::NodeConstStruct);
			 context->HelperSetSystemIdtf("hamilton cycle for '" + context->HelperGetSystemIdtf(structNode) + "'", answer);
				
				ScAddr arc = context->CreateEdge(ScType::EdgeDCommonConst, startNode, answer);
				context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_answer, arc);

				return answer;
}





ScAddr createAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr structNode, ScAddr startNode, vector<ScAddr> &path) {
				ScAddr answer = context->HelperFindBySystemIdtf("hamilton cycle for '" + context->HelperGetSystemIdtf(structNode) + "'");

				if (answer.IsValid()) {
							SC_LOG_DEBUG("answer exist DELETING: " + context->HelperGetSystemIdtf(answer));	
					  deleteAnswer(context, answer);
				} else {
						 answer = createEmptyAnswer(context, structNode, startNode);
							SC_LOG_DEBUG("answer not exist CREATING: " + context->HelperGetSystemIdtf(answer));	
				}

				addToAnswer(context, answer, path);

				return answer;
}


void addToAnswer(const std::unique_ptr<ScMemoryContext>& context, ScAddr answer, vector<ScAddr> &path) {
				ScAddr answerArc;
				for (int i = 0; i< (path.size()-1); i++) {
							ScAddr node1 = path[i];
							ScAddr node2 = path[i+1];
		
							context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, node1);
							answerArc = context->CreateEdge(ScType::EdgeDCommonConst, node1, node2);

							context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, answerArc);
					}

					answerArc = context->CreateEdge(ScType::EdgeDCommonConst, path.back(), path.front());
					context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, answerArc);
}




} // namespace exampleModul