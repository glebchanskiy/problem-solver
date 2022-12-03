/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_addr.hpp>

#include "keynodes.generated.hpp"

namespace testModule
{

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("find_gamelton_cicle"), ForceCreate)
  static ScAddr find_gamelton_cicle;

  SC_PROPERTY(Keynode("nrel_answer"), ForceCreate)
  static ScAddr nrel_answer;

  SC_PROPERTY(Keynode("system_element"), ForceCreate)
  static ScAddr system_element;

  SC_PROPERTY(Keynode("question_finished"), ForceCreate)
  static ScAddr question_finished;

  SC_PROPERTY(Keynode("rrel_key_sc_element"), ForceCreate)
  static ScAddr rrel_key_sc_element;
  

};

} // namespace exampleModule
