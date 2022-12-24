/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_addr.hpp>

#include "keynodes.generated.hpp"

namespace courseWorkNamespace
{

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("question_find_longest_path"), ForceCreate)
  static ScAddr question_find_longest_path;

  SC_PROPERTY(Keynode("question_search_biconnected_graph"), ForceCreate)
  static ScAddr question_search_biconnected_graph;

  SC_PROPERTY(Keynode("question_search_cactus_graph"), ForceCreate)
  static ScAddr question_search_cactus_graph;

  SC_PROPERTY(Keynode("question_search_edge_connectivity"), ForceCreate)
  static ScAddr question_search_edge_connectivity;

  SC_PROPERTY(Keynode("question_search_hamilton_cycle"), ForceCreate)
  static ScAddr question_search_hamilton_cycle;

  SC_PROPERTY(Keynode("question_search_path_in_graph"), ForceCreate)
  static ScAddr question_search_path_in_graph;

  SC_PROPERTY(Keynode("question_find_max_and_middle_path"), ForceCreate)
  static ScAddr question_find_max_and_middle_path;



  SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
  static ScAddr nrel_answer;

  SC_PROPERTY(Keynode("rel_first_vertex"), ForceCreate)
  static ScAddr rel_first_vertex;

  SC_PROPERTY(Keynode("rel_start_vertex"), ForceCreate)
  static ScAddr rel_start_vertex;

  SC_PROPERTY(Keynode("rel_end_vertex"), ForceCreate)
  static ScAddr rel_end_vertex;

  SC_PROPERTY(Keynode("rel_central_1"), ForceCreate)
  static ScAddr rel_central_1;

  SC_PROPERTY(Keynode("rel_central_2"), ForceCreate)
  static ScAddr rel_central_2;
};

} // namespace courseWorkNamespace
