/** ---*- C++ -*--- DepPrinter.cpp
 *
 * Copyright (C) 2012 Marco Minutoli <mminutoli@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

//#include "cot/DependencyGraph/DependencyGraph.h"

#include "DependencyGraph.h"

#include "AllPasses.h"
#include "DataDependencies.h"
#include "ControlDependencies.h"
#include "ProgramDependencies.h"
//#include "SystemControlDependenceGraph.h"
//#include "SystemDataDependencies.h"
#include "llvm/Analysis/DOTGraphTraitsPass.h"


using namespace llvm;
using namespace cot;


namespace llvm {

  template <>
  struct DOTGraphTraits<cot::DepGraphNode *> : public DefaultDOTGraphTraits {
    DOTGraphTraits (bool isSimple = false): DefaultDOTGraphTraits(isSimple) {}

    std::string getNodeLabel(cot::DepGraphNode *Node, cot::DepGraphNode *Graph){

      const InstructionWrapper *instW = Node->getData();

      //TODO: why nullptr for Node->getData()?
      if(instW == nullptr){
	errs() <<"instW " << instW << "\n";
	return "null instW";
      }




      std::string Str;
      raw_string_ostream OS(Str);

      switch(instW->getType()) {
      case ENTRY:
	return ("<<ENTRY>> " + instW->getFunctionName());

      case GLOBAL_VALUE:{
	OS << *instW->getValue();
	return ("GLOBAL_VALUE:" + OS.str());
      }

      case FORMAL_IN:{
	OS << *instW->getArgument()->getType();
	return ("FORMAL_IN:" + OS.str());
      }

      case ACTUAL_IN:{
	OS << *instW->getArgument()->getType();
	return ("ACTUAL_IN:" + OS.str() );
      }
      case FORMAL_OUT:{
	OS << *instW->getArgument()->getType();
	return ("FORMAL_OUT:" + OS.str());
      }

      case ACTUAL_OUT:{
	OS << *instW->getArgument()->getType();
	return ("ACTUAL_OUT:" + OS.str());
      }

      case PARAMETER_FIELD:{
	OS << instW->getFieldId() << " " << *instW->getFieldType();
	return OS.str();
      }

	//for pointer node, add a "*" sign before real node content
	//if multi level pointer, use a loop instead here
      case POINTER_RW:{
	OS << *instW->getArgument()->getType();
	return ("POINTER READ/WRITE : *" + OS.str());
      }

	break;
      }

      const Instruction *inst = Node->getData()->getInstruction();

      if (isSimple() && !inst->getName().empty()) {
	return inst->getName().str();
      } else {
	std::string Str;
	raw_string_ostream OS(Str);
	OS << *inst;
	return OS.str();}
    }
  };

  template <>
  struct DOTGraphTraits<cot::DepGraph *>
    : public DOTGraphTraits<cot::DepGraphNode *>
  {
    DOTGraphTraits (bool isSimple = false)
      : DOTGraphTraits<cot::DepGraphNode *>(isSimple) {}

    std::string getNodeLabel(cot::DepGraphNode *Node, cot::DepGraph *Graph){
      return DOTGraphTraits<DepGraphNode *>
	::getNodeLabel(Node,*(Graph->begin_children()));}
  };


  template <>
  struct DOTGraphTraits<cot::DataDependencyGraph *>
    : public DOTGraphTraits<cot::DepGraph *>
  {
    DOTGraphTraits (bool isSimple = false)
      : DOTGraphTraits<cot::DepGraph *>(isSimple) {}

    static std::string getGraphName(DataDependencyGraph *){
      return "Data dependency graph";}

    std::string getNodeLabel(cot::DepGraphNode *Node, cot::DataDependencyGraph *Graph){
      return DOTGraphTraits<DepGraph *>
	::getNodeLabel(Node, Graph->DDG);}
  };
  /*
    template <>
    struct DOTGraphTraits<cot::SystemDataDependencyGraph *>
    : public DOTGraphTraits<cot::DepGraph *>
    {
    DOTGraphTraits (bool isSimple = false)
    : DOTGraphTraits<cot::DepGraph *>(isSimple) {}

    static std::string getGraphName(cot::SystemDataDependencyGraph *){
    return "SystemData dependency graph";}

    std::string getNodeLabel(cot::DepGraphNode *Node, cot::SystemDataDependencyGraph *Graph){
    return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->SystemDG);}
    };*/
 
  template <>
  struct DOTGraphTraits<cot::ControlDependencyGraph *>
    : public DOTGraphTraits<cot::DepGraph *>
  {
    DOTGraphTraits (bool isSimple = false)
      : DOTGraphTraits<cot::DepGraph *>(isSimple) {}

    static std::string getGraphName(ControlDependencyGraph *){
      return "Instruction-Level Control dependency graph";}

    std::string getNodeLabel(cot::DepGraphNode *Node,
			     cot::ControlDependencyGraph *Graph){
      return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->CDG);}

    static std::string getEdgeSourceLabel(cot::DepGraphNode *Node, cot::DependencyLinkIterator<InstructionWrapper> EI) {
      //    errs() << "getEdgeSourceLabel(): type = " << EI.getDependencyType() << "\n";
      switch (EI.getDependencyType()) {

      default: return "";}
    }
  };

  template <>
  struct DOTGraphTraits<cot::ProgramDependencyGraph *>
    : public DOTGraphTraits<cot::DepGraph *>
  {
    DOTGraphTraits (bool isSimple = false)
      : DOTGraphTraits<cot::DepGraph *>(isSimple) {}

    static std::string getGraphName(ProgramDependencyGraph *){
      return "Program Dependency Graph";
    }

    std::string getNodeLabel(cot::DepGraphNode *Node, cot::ProgramDependencyGraph *Graph){
      return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->PDG);
    }


     //return IW.getDependencyType() == DATA ?
      //"style=dotted" : "";
 
      //take care of the probable display error here

    std::string getEdgeAttributes(cot::DepGraphNode *Node,
				  cot::DependencyLinkIterator<InstructionWrapper> &IW,
				  cot::ProgramDependencyGraph *PD){

      switch(IW.getDependencyType())
	{
	case CONTROL:
	  return "";
	case DATA_GENERAL:
	  return "style=dotted";
	case GLOBAL_VALUE:
	  return "style=dotted";
	case PARAMETER:
	  return "style=dashed";
	case DATA_DEF_USE:{
	  Instruction* pFromInst = Node->getData()->getInstruction();
	  return "style=dotted,label = \"{DEF_USE}\" ";
	}
	case DATA_RAW:{

	  Instruction* pInstruction = IW.getDependencyNode()->getInstruction();
	  //pTo Node must be a LoadInst
	  string ret_str;
	  if(isa<LoadInst>(pInstruction)){
	    LoadInst* LI = dyn_cast<LoadInst>(pInstruction);
	    Value* valLI = LI->getPointerOperand();
	    ret_str = "style=dotted,label = \"{RAW} " + valLI->getName().str() + "\"";
	  }
	  else if(isa<CallInst>(pInstruction)){
      	    ret_str = "style=dotted,label = \"{RAW}\"";
	  }
	  else
	    errs() << "incorrect instruction for DATA_RAW node!" << "\n";
	  return ret_str;	  
	}	
	}//end switch       
      return ""; //default ret statement
    }//end getEdgeAttr...
  };
}// end namespace llvm


namespace cot{

  namespace {
    struct DataDependencyViewer
      : public DOTGraphTraitsViewer<DataDependencyGraph, false>{
      static char ID;
      DataDependencyViewer() :
	DOTGraphTraitsViewer<DataDependencyGraph, false>("ddgraph", ID) {} };

    struct ControlDependencyViewer
      : public DOTGraphTraitsViewer<ControlDependencyGraph, false>{
      static char ID;
      ControlDependencyViewer() :
	DOTGraphTraitsViewer<ControlDependencyGraph, false>("cdgraph", ID) {} };

    struct ProgramDependencyViewer
      : public DOTGraphTraitsViewer<ProgramDependencyGraph, false>{
      static char ID;
      ProgramDependencyViewer() :
	DOTGraphTraitsViewer<ProgramDependencyGraph, false>("pdgraph", ID) {} };
  }
}
 



char cot::DataDependencyViewer::ID = 0;
static RegisterPass<cot::DataDependencyViewer> DdgViewer("view-ddg", "View data dependency graph of function", false, false);


char cot::ControlDependencyViewer::ID = 0;
static RegisterPass<cot::ControlDependencyViewer> CDGViewer("view-cdg", "View control dependency graph of function", false, false);

 
char cot::ProgramDependencyViewer::ID = 0;
static RegisterPass<cot::ProgramDependencyViewer> PdgViewer("view-pdg", "View program dependency graph of function", false, false);





namespace cot
{
  namespace {
    struct DataDependencyPrinter
      : public DOTGraphTraitsPrinter<DataDependencyGraph, false>
    {
      static char ID;
      DataDependencyPrinter()
	: DOTGraphTraitsPrinter<DataDependencyGraph, false>("ddgragh", ID) {}
    };

    /*  
    
	struct SystemControlDependencyPrinter
	: public DOTGraphTraitsModulePrinter<SystemControlDependenceGraph, false, SystemControlDependenceGraph *>
	{
	static char ID;
	SystemControlDependencyPrinter()
	: DOTGraphTraitsModulePrinter<SystemControlDependenceGraph, false, SystemControlDependenceGraph *>("scdgragh", ID) {}
	};    



	struct SystemDataDependencyPrinter
	: public DOTGraphTraitsPrinter<SystemDataDependencyGraph, false>
	{
	static char ID;
	SystemDataDependencyPrinter()
	: DOTGraphTraitsPrinter<SystemDataDependencyGraph, false>("sdgragh", ID) {}
	};*/
      
    
    struct ControlDependencyPrinter
      : public DOTGraphTraitsPrinter<ControlDependencyGraph, false>
    {
      static char ID;
      ControlDependencyPrinter()
	: DOTGraphTraitsPrinter<ControlDependencyGraph, false>("cdgragh", ID) {}
    };

    struct ProgramDependencyPrinter
      : public DOTGraphTraitsPrinter<ProgramDependencyGraph, false>
    {
      static char ID;
      ProgramDependencyPrinter()
	: DOTGraphTraitsPrinter<ProgramDependencyGraph, false>("pdgragh", ID) {}
    };

  }
}


char cot::DataDependencyPrinter::ID = 0;
static RegisterPass<cot::DataDependencyPrinter> DdGPrinter("dot-ddg", "Print data dependency graph of function to 'dot' file", false, false);

/*
  char cot::SystemControlDependencyPrinter::ID = 0;
  static RegisterPass<cot::SystemControlDependencyPrinter> SCDGPrinter("dot-scdg", "Print instruction-level system control dependency graph of function to 'dot' file", false, false);

  
  char cot::SystemDataDependencyPrinter::ID = 0;
  static RegisterPass<cot::SystemDataDependencyPrinter> SDGPrinter("dot-sdg", "Print System data dependency graph of function to 'dot' file", false, false);
*/

//useless
/*
  struct SystemControlDependencyPrinter
  : public DOTGraphTraitsModulePrinter<SystemControlDependenceGraph, false, SystemControlDependenceGraph *>
  {
  static char ID;
  SystemControlDependencyPrinter()
  : DOTGraphTraitsModulePrinter<SystemControlDependenceGraph, false, SystemControlDependenceGraph *>("scdgragh", ID) {}
  };
*/

char cot::ControlDependencyPrinter::ID = 0;
static RegisterPass<cot::ControlDependencyPrinter> CDGPrinter("dot-cdg", "Print control dependency graph of function to 'dot' file", false, false);


char cot::ProgramDependencyPrinter::ID = 0;
static RegisterPass<cot::ProgramDependencyPrinter> PDGPrinter("dot-pdg", "Print instruction-level program dependency graph of function to 'dot' file", false, false);




/*
  if(IW.getDependencyType() == CONTROL)
  return "";
  else if(IW.getDependencyType() == DATA_GENERAL)
  return "style=dotted";
  else if(IW.getDependencyType() == DATA_DEF_USE)
  {
  Instruction* pFromInst = Node->getData()->getInstruction();
  //	  errs() << "pFromInst: "<< *pFromInst << "\n";
	  
  return "style=dotted,label = \"{DEF_USE}\" ";		  
  }

  else
  { 
  errs() << "only test whether enter..." << "\n";

  Instruction* pInstruction = IW.getDependencyNode()->getInstruction();
  //pTo Node must be a LoadInst

  errs() << "inst = " << *pInstruction << "address = " << pInstruction << "\n";

  LoadInst* LI = dyn_cast<LoadInst>(pInstruction);
  Value* valLI = LI->getPointerOperand();

  string ret_str = "style=dotted,label = \"{RAW} " + valLI->getName().str() + "\"";

  errs() << "only test whether leave..." << "\n\n" ;

  return ret_str;	  
  }
*/
/*
  template <>
  struct DOTGraphTraits<cot::SystemControlDependenceGraph *>
  : public DOTGraphTraits<cot::DepGraph *> {
  DOTGraphTraits (bool isSimple = false) : DOTGraphTraits<cot::DepGraph *>(isSimple) {}
  static std::string getGraphName(SystemControlDependenceGraph *Graph) {
  return "Intruction-Level System control dependence graph";
  }

  std::string getNodeLabel(cot::DepGraphNode *Node, cot::SystemControlDependenceGraph *Graph)
  {
  return DOTGraphTraits<DepGraph *>::getNodeLabel(Node, Graph->SCDG);
  }

  static std::string getEdgeSourceLabel(cot::DepGraphNode *Node, cot::DependencyLinkIterator<InstructionWrapper> EI) {
  //errs() << "getEdgeSourceLabel(): type = " << EI.getDependencyType() << "\n";
  switch (EI.getDependencyType()) {
  default: return "";}
  }
  };*/
