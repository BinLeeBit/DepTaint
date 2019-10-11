/** ---*- C++ -*--- DataDependencies.cpp
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

//#include "cot/DependencyGraph/DataDependencies.h"

#include "DataDependencies.h"
#include "llvm/IR/DebugInfo.h"

#include "AllPasses.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/SmallVector.h"

#include "llvm/Analysis/MemoryBuiltins.h"


using namespace cot;
using namespace llvm;
using namespace std;

char DataDependencyGraph::ID = 0;

//std::set<InstructionWrapper *> InstructionWrapper::nodes;
int maxLocNum = 100000;
map<Instruction*, int> loadLocMap;

unsigned unsigned_abs(unsigned a, unsigned b){
  if (a >= b)
    return a - b;
  else
    return b - a;
}


bool DataDependencyGraph::runOnFunction(llvm::Function &F)
{
  errs() << "++++++++++++++++++++++++++++++ DataDependency::runOnFunction +++++++++++++++++++++++++++++" << '\n'; 
  errs() << "Function name:" << F.getName().str() << '\n';

  InstructionWrapper::constructInstMap(F);
  //  AliasAnalysis &AA = getAnalysis<AliasAnalysis>();
  FlowDependenceAnalysis& MDA = getAnalysis<FlowDependenceAnalysis>();

  errs() << "After getAnalysis<FlowDependenceAnalysis>()" << '\n';


  for(BasicBlock &B : F){
    for(Instruction &I : B){
      if(isa<LoadInst>(&I)){
	errs() << "LoadInst found in DDG: " << I << "\n";
	loadLocMap[&I] = maxLocNum;
      }
    }
  }




  for(inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E; ++instIt){

    DDG->getNodeByData(InstructionWrapper::instMap[&*instIt]);

    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);
    

    //    errs() << "Inst = " << *pInstruction << '\n';

    /*
    if(isa<CallInst>(pInstruction)){
      errs() << "isa <CallInst>: " << *pInstruction << '\n';

      CallInst* cInstruction = dyn_cast<CallInst>(&*instIt);
      Function* call_func = cInstruction->getCalledFunction();
      
      errs() << "funcion called : " << call_func->getName() << '\n';

      //      AttributeSet AS = call_func->getAttributes();

      //      errs() << "********arg_size = " << call_func->arg_size() << '\n';


      for(Function::arg_iterator ai = call_func->arg_begin(); ai != call_func->arg_end(); ++ai)
	{
	  
	  Value*  x = ai;
	  x->setName(ai->getName());
	  errs() << "call_func args = " << *x << '\n'; 
	  
	  InstructionWrapper *instW = new InstructionWrapper(call_func, x, FORMAL_IN);

	  //	  DDG->addDependency(InstructionWrapper::instMap[pInstruction], );
	}

    }*/


    if(isa<LoadInst>(pInstruction)){

      //      errs() << "isa <LoadInst> : --------------------------------------" << '\n';
      std::vector<Instruction*> flowdep_set = MDA.getDependencyInFunction(F, pInstruction);
	
      Instruction* candidateSI = nullptr;
      for(int i = 0; i < flowdep_set.size(); i++){
	//	  errs() << "return storeisnt = " << *flowdep_set[i] << '\n';
	//DD direction: store --> Load

	int diffLoadStore =  unsigned_abs(flowdep_set[i]->getDebugLoc().getLine(), pInstruction->getDebugLoc().getLine());

	//	errs() << "1 arg " <<   flowdep_set[i]->getDebugLoc().getLine() << "\n";
	//	errs() << "2 arg " << pInstruction->getDebugLoc().getLine() << "\n";

	//	errs() << "LI loc right now: " <<loadLocMap[pInstruction]<< " " <<*pInstruction<< "\n"; 
	//	errs() << "SI loc right now: " <<diffLoadStore << " " <<*flowdep_set[i]<<"\n";
	if(loadLocMap[pInstruction] > diffLoadStore){
	  loadLocMap[pInstruction] = diffLoadStore;
	  candidateSI = flowdep_set[i];
	  //	  errs() << "candidateSI: " << *candidateSI << "LI:" << *pInstruction << "\n";
	}
	//	else{
	//  errs() << "not qualified! loadLocMap[]="<<loadLocMap[pInstruction]<<"\n";
	//	}
      }

      //      errs() << "LI:" << *pInstruction << "addr:" << pInstruction << "\n";
      //  if(candidateSI != nullptr)
      //      errs() << "SI:" << *candidateSI << "\n";
	DDG->addDependency(InstructionWrapper::instMap[candidateSI], InstructionWrapper::instMap[pInstruction], DATA_RAW);
	
      flowdep_set.clear();
    } 
	
    // Def-Use(deal with register values) Data dependency between temporaries. It's easy to detect a DD between
    // temporaries because LLVM uses the SSA form. So in orderd to detect a DD,
    // it suffices to find all operands in an instruction and add a dependency between it and the one that defines the operand.
    for (Instruction::const_op_iterator cuit = instIt->op_begin(); cuit != instIt->op_end(); ++cuit)
      if(Instruction* pInstruction = dyn_cast<Instruction>(*cuit)){
	Value* tempV = dyn_cast<Value>(*cuit);
	//	errs() << "@@@@@@@ addDependency " << *pInstruction << " --> " << *instIt << '\n';

	/*
	if(pInstruction != nullptr && isa<CmpInst>(pInstruction)){
	  errs() << "DDG CmpInst: " << *pInstruction << "\n";
	  continue;
	  }*/

	DDG->addDependency(InstructionWrapper::instMap[pInstruction], InstructionWrapper::instMap[&*instIt], DATA_DEF_USE);
      }
  }//end for iterator instIt

  return false;
}

void DataDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const{
  //  AU.addRequiredTransitive<AliasAnalysis>();
  AU.addRequiredTransitive<FlowDependenceAnalysis>();
  AU.setPreservesAll();
}

void DataDependencyGraph::print(raw_ostream &OS, const Module*) const{
  DDG->print(OS, getPassName());
}


DataDependencyGraph *cot::CreateDataDependencyGraphPass(){
  return new DataDependencyGraph();
}


static RegisterPass<cot::DataDependencyGraph> DDG("ddg", "Data Dependency Graph Construction", false, true);


/*
  INITIALIZE_PASS(DataDependencyGraph, "ddg",
  "Data Dependency Graph Construction",
  true,
  true)
*/


//	SmallVector<NonLocalDepResult,20> result;

//	AliasAnalysis::Location Loc = AA.getLocation(dyn_cast<LoadInst>(pInstruction));
//	bool isLoad = true;
//MDA.getNonLocalPointerDependency(Loc,isLoad, pInstruction->getParent(),result);

//	errs() << "Common Def-Use,  QueryInst: " << *pInstruction << " pInstaddr = " << pInstruction << " Dependence: " << *instIt << "instIt = " << &*instIt << " operand: " << *tempV << " tempV = " << tempV << '\n';


/*	
	errs() << "pInstruction" << *pInstruction << '\n';
	SmallVector<NonLocalDepResult,20> result;

	AliasAnalysis::Location Loc = AA.getLocation(dyn_cast<LoadInst>(pInstruction));
	bool isLoad = true;

	BasicBlock* BB = pInstruction->getParent();
	bool depInstFound = false;
			
	while(BB != nullptr && depInstFound == false){
	errs() <<"BB: " << BB->getName() <<'\n';

	MDA.getNonLocalPointerDependency(Loc,isLoad,BB,result);

	//AliasAnalysis::ModRefResult 
	for(BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I){
	AliasAnalysis::Location MemLoc_in_BB;
	GetLocation(&*I, MemLoc_in_BB, &AA);
	if(Loc.Ptr == MemLoc_in_BB.Ptr){
	errs() << "Loc.Ptr == MemLoc_in_BB.Ptr" << *I << '\n';

	InstructionWrapper *itInst = InstructionWrapper::instMap[pInstruction];
	InstructionWrapper *parentInst = InstructionWrapper::instMap[&*I];
	DDG->addDependency(itInst, parentInst, DATA);
	depInstFound = true;	      
	}
	}	


	for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI){
	if(BI == F.begin()){
	if(BB == &*BI) {BB = nullptr; break;}
	}
	else{
	if(BB == &*BI) {BB = &*(--BI); break;}
	}
	}

	//BB = BB->getUniquePredecessor();

	if(BB == nullptr)
	errs() << "BB_UniquePredecessor is nullptr!" << '\n';
	else errs() << "Now BB = " << BB->getName() << '\n';

	}//end while BB
	
	errs() << "pInstruction done!" << *pInstruction << '\n';
	  
*/ //end res.isNonLocal()){


/*
  AliasAnalysis:: ModRefResult MRR = AA.getModRefInfo(&*I, Loc);
  if(MRR == llvm::AliasAnalysis::Mod){
  //|| MRR == llvm::AliasAnalysis::ModRef
  errs() << "Mod: " << *I << '\n';

  InstructionWrapper *itInst = InstructionWrapper::instMap[&*instIt];
  InstructionWrapper *parentInst = InstructionWrapper::instMap[&*I];

  DDG->addDependency(itInst, parentInst, DATA);
	      
  depInstFound = true;
  }*/

/*
  for(SmallVector<NonLocalDepResult,20>::iterator II = result.begin(), EE = result.end();II != EE;++II){

  errs() << "SmallVecter size = " << result.size() << '\n';
  FlowDepResult nonLocal_res = II->getResult();
  InstructionWrapper *itInst = InstructionWrapper::instMap[&*instIt];
  InstructionWrapper *parentInst = InstructionWrapper::instMap[nonLocal_res.getInst()];

  errs() << II->getBB()->getName()<<'\n';

  if(nullptr != nonLocal_res.getInst()){
  errs() << "nonLocal_res.getInst(): " << *nonLocal_res.getInst() << '\n';
  DDG->addDependency(itInst, parentInst, DATA);
  }
  else
  ;
  //	      errs() << "nonLocal_res.getInst() is a nullptr"<<'\n';
  }
*/

/*
  else if(res.isNonLocal()){

  InstructionWrapper *itInst = InstructionWrapper::instMap[&*instIt];
  InstructionWrapper *parentInst = InstructionWrapper::instMap[res.getInst()];

  Instruction* inst = res.getInst();
  errs() << "isa Store Inst: " << *inst << '\n';
  DDG->addDependency( itInst, parentInst, DATA);
  }
*/
//    Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);
// Instruction* temp = res.getInst();
// Instruction* inst = dyn_cast<Instruction>(*res.getInst());
/* 
   if(isa<StoreInst>(inst)){
   errs() << "isa Store Inst: " << *inst << '\n';
   //	DDG->addDependency( parentInst, itInst, DATA);
   }*/



//     MemDepResult res getDependency(Instruction *QueryInst);

/*
  for(SmallVector<NonLocalDepResult,20>::iterator II = result.begin(), EE = result.end();II != EE;++II){
  errs() << "SmallVector iterator Address:" << II->getAddress() << "Loc->Ptr: " << Loc.Ptr <<'\n';
  }
*/
/*	    
	    if(nullptr != II->getResult().getInst()){
	    Instruction* tempI =  dyn_cast<Instruction>(II->getResult().getInst());
	    errs() << "tempI = " << *tempI << '\n';
	    }*/
//	errs() << "II->getResult().getInst()" << *dyn_cast<Instruction>(II->getResult().getInst()) << '\n';
/*
  else
  errs() << "II->getResult().getInst() == nullptr!" << '\n';*/
      



/*
  FlowDepResult res = MDA.getDependency(pInstruction);

  if(res.isDef() && nullptr != res.getInst()){
  errs() << "Def LoadInst!" << '\n';

  InstructionWrapper *itInst = InstructionWrapper::instMap[&*instIt];
  InstructionWrapper *parentInst = InstructionWrapper::instMap[res.getInst()];
  DDG->addDependency(itInst, parentInst, DATA);
  }

  if(res.isNonLocal()){

  }//end if res.isNonLocal
*/


/*
  Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);

  if(isa<LoadInst>(pInstruction)) 
  {  
  errs() << "isa LoadInst: " << *pInstruction << "addr: " << pInstruction << '\n';
  LoadInst* LI = dyn_cast<LoadInst>(pInstruction);

  //reiterate the function and find the dependency
  for(inst_iterator instIt2 = inst_begin(F), E2 = inst_end(F); instIt2 != E2; ++instIt2)
  {
  if(instIt2 == instIt) {}//LoadInst itself found, do nothing
  else
  {
  Instruction *pInstruction2 = dyn_cast<Instruction>(&*instIt2);
  if(isa<StoreInst>(pInstruction2)){
  //errs() << "isa<StoreInst> : " << *pInstruction2 << "addr: " << &*instIt2  << '\n';
  StoreInst* SI = dyn_cast<StoreInst>(pInstruction2);
  Value* valSI = SI->getPointerOperand();
	    
  if(SI->getPointerOperand() == LI->getPointerOperand()){
  errs() << "flow-sensitive dependence found : " << *SI << "  " << *LI << '\n';
  DDG->addDependency(InstructionWrapper::instMap[SI], InstructionWrapper::instMap[LI], DATA);
  }
  }
  }//end else
  }//end reiterate the function

	
  //process alias
  SmallVector<NonLocalDepResult,20> result;
  AliasAnalysis::Location loadLoc = AA.getLocation(LI);
  //	errs() << "alias Loc.Ptr = " << Loc

  bool isLoad = true;

  BasicBlock* BB = pInstruction->getParent();
  MDA.getNonLocalPointerDependency(loadLoc,isLoad,BB,result);


	


  // If we found a pointer, check if it could be the same as our pointer.
  // AliasAnalysis::AliasResult R = AA->alias(LoadLoc, MemLoc);

	
  for(SmallVector<NonLocalDepResult,20>::iterator II = result.begin(), EE = result.end();II != EE;++II){
  errs() << "SmallVecter size = " << result.size() << '\n';


	  
  FlowDepResult nonLocal_res = II->getResult();
  InstructionWrapper *itInst = InstructionWrapper::instMap[&*instIt];
  InstructionWrapper *parentInst = InstructionWrapper::instMap[nonLocal_res.getInst()];
	  
  errs() << "II->getBB()->getName() " << II->getBB()->getName()<<'\n';

  }

	




  errs() << "+++++++++++++++++++++++++++++++++++++++" <<'\n';
  }//end if LoadInst

*/
