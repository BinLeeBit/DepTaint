#include "Config.h"

#include <unistd.h>

//#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DebugInfo.h"


#include "llvm/Bitcode/ReaderWriter.h"

#include "ConnectFunctions.h"
#include "FunctionWrapper.h"
#include "ProgramDependencies.h"
#include "SystemDataDependencies.h"
#include "SystemControlDependenceGraph.h"

#include "AllPasses.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Intrinsics.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include <map>
#include <vector>
#include <deque>
#include <list>
#include <iostream>
#include <fstream>

#include <string.h>
#include <time.h>

using namespace std;
using namespace cot;
using namespace llvm;

typedef struct S{
  int val; 
  struct S* next;
}tnameS;

/*
  typedef struct {
  string val;
  unsigned lineNum;
  }TaintValue;

  class rule:greater<TaintValue>
  {
  public:
  bool operator () (TaintValue p1,TaintValue p2) const{
  return p1.lineNum < p2.lineNum;
  }
  };*/

//unsigned getLine() const in DebugLoc.h
std::map<llvm::Value*, std::set<string> > taintMap;
//map each alloca into a status(bool), if true means this alloca now is sensitive,otherwise not

const unsigned allocaMAX = 100000;
//std::map<llvm::Instruction*, unsigned> allocaMap;
std::set<string> fixedTaintValues;

static IRBuilder<> Builder(getGlobalContext());

static int bid = 0;

//a dictionary, key 
map<BasicBlock*, int> bbOrderMap;

//a dictionary, key is a BB, set is all instructions in this BB  
map<BasicBlock*, set<Instruction*> > bbInstMap;





/// FindFunctionBackedges - Analyze the specified function to find all of the
/// loop backedges in the function and return them.  This is a relatively cheap
/// (compared to computing dominators and loop info) analysis.
///
/// The output is added to Result, as pairs of <from,to> edge info.

void FindFunctionBackedges(const Function &F,
			   SmallVectorImpl<std::pair<const BasicBlock*,const BasicBlock*> > &Result) {
  const BasicBlock *BB = &F.getEntryBlock();
  if (succ_begin(BB) == succ_end(BB))
    return;

  SmallPtrSet<const BasicBlock*, 8> Visited;
  SmallVector<std::pair<const BasicBlock*, succ_const_iterator>, 8> VisitStack;
  SmallPtrSet<const BasicBlock*, 8> InStack;

  Visited.insert(BB);
  VisitStack.push_back(std::make_pair(BB, succ_begin(BB)));
  InStack.insert(BB);
  do {
    std::pair<const BasicBlock*, succ_const_iterator> &Top = VisitStack.back();
    const BasicBlock *ParentBB = Top.first;
    succ_const_iterator &I = Top.second;

    bool FoundNew = false;
    while (I != succ_end(ParentBB)) {
      BB = *I++;
      if (Visited.insert(BB)) {
	FoundNew = true;
	break;
      }
      // Successor is in VisitStack, it's a back edge.
      if (InStack.count(BB))
	Result.push_back(std::make_pair(ParentBB, BB));
    }

    if (FoundNew) {
      // Go down one level if there is a unvisited successor.
      InStack.insert(BB);
      VisitStack.push_back(std::make_pair(BB, succ_begin(BB)));
    } else {
      // Go up one level.
      InStack.erase(VisitStack.pop_back_val().first);
    }
  } while (!VisitStack.empty());
}

void removeBackedgesInFunction(Function &F){
  SmallVector<std::pair<const BasicBlock*,const BasicBlock*>, 32> Edges;

  FindFunctionBackedges(F, Edges);

  errs() << "Back Edges = " << Edges.size() << "\n";
  std::deque<BasicBlock*> emptyBBqueue;

  //for each backedge, 
  for (auto I : Edges){
    BasicBlock *BBFrom = const_cast<BasicBlock*>(I.first); 
    TerminatorInst *TermBBFrom = BBFrom->getTerminator();

    BasicBlock *InsertBefore = std::next(Function::iterator(*BBFrom)).getNodePtrUnchecked();
    BasicBlock *BBNew = BasicBlock::Create(BBFrom->getContext(), "empty.else", BBFrom->getParent(), InsertBefore);

    TermBBFrom->eraseFromParent();

    if(emptyBBqueue.empty())
      BranchInst *BI = BranchInst::Create(BBNew, BBFrom);
    else
      BranchInst *BI = BranchInst::Create(emptyBBqueue.back(), BBFrom);

    // Add a branch instruction to the newly formed basic block.
    //BranchInst *BI = BranchInst::Create(New, this);
    emptyBBqueue.push_back(BBNew);
  }

  errs() << "for I : Edges" << "\n";

  //BBFrom is the starting empty basicblock we created
  BasicBlock *BBFrom = NULL;
  if(!emptyBBqueue.empty()){
    BBFrom = emptyBBqueue.front(); 
    emptyBBqueue.pop_front();
  }
  //Keep connecting each the empty else BB we created before, between each pair of inner and outer loops
  while (!emptyBBqueue.empty()){
    BasicBlock *BBTo = emptyBBqueue.front();
    emptyBBqueue.pop_front();
    BranchInst *BI = BranchInst::Create(BBTo, BBFrom);
    BBFrom = BBTo;
  }

  //TODO:UnifyFunctionExitNodes required here to ensure that one function has at most one return instruction in it.
  //Get "pseduo terminator" basicblock
  //Remember BBFrom is the last empty else basicblock

  //find the last basicblock in function F
  BasicBlock *pseduoTerminatorBB = nullptr;
  Function::iterator I = F.end();
  if (I == F.begin())
    errs() << "no basicblocks at all in " << F.getName() << " \n";
  else
    pseduoTerminatorBB = &*--I;

  errs() << "before BBfrom-getInstList\n";

  //move instructions from pseduoTerminatorBB to BBFrom, splice is an API for moving instructions across BasicBlocks
  //   BBFrom->getInstList().splice(BBFrom->begin(), pseduoTerminatorBB->getInstList(), pseduoTerminatorBB->begin());
    
  errs() << "after splice\n";

  //create a br instruction in the pseduoTerminator BB, make it points to the final empty.else BB 
  //e.g.: ret i32 0 ==> br label %empty.else1
  //
  BranchInst *BI = BranchInst::Create(BBFrom, pseduoTerminatorBB);

  errs() << "pseduoTerminatorBB = " << *pseduoTerminatorBB << "\n";

  errs() << "dump Module:\n";
  //    errs() << *F.getParent() << "\n";
    
}






void printTaintValues(Function& F, ostream& OS){
  unsigned prev = 0;
  bool endLabel = false;
  for (BasicBlock &B : F){
    for (Instruction &I : B){
      unsigned iPos = I.getDebugLoc().getLine();
      if(iPos == 0 && prev != 0){
	endLabel = true;
	continue;
      }
      else if (iPos > 0 && iPos != prev){	
	//skip callinst including void @llvm.dbg.declare(...)
	if(isa<CallInst>(I)){
	  continue;
	}
	errs() << I.getDebugLoc().getLine() << " " << I <<"\n";
	
	OS << iPos << ":";
	for(auto const& pv : FunctionWrapper::funcMap[&F]->getAllocaMap()){
	  if(pv.second <= iPos){
	    errs() << pv.first->getName() << ",";
	    OS << pv.first->getName().str() << ",";
	  }
	}
	errs() << "\n";
	OS << "\n";
	prev = iPos;
      }
    }
    //    if(endLabel) break;
  }
}


char ProgramDependencyGraph::ID = 0;
//std::map<const llvm::BasicBlock *,BasicBlockWrapper *> BasicBlockWrapper::bbMap;
AliasAnalysis* ProgramDependencyGraph::Global_AA = nullptr;

std::map<const llvm::Function *,FunctionWrapper *> FunctionWrapper::funcMap;
std::map<const llvm::CallInst *,CallWrapper * > CallWrapper::callMap;

std::set<llvm::Value*> allPtrSet;
std::vector<llvm::Value*> sensitive_values;
std::vector<InstructionWrapper*> sensitive_nodes;


void ProgramDependencyGraph::connectAllPossibleFunctions(InstructionWrapper* CInstW, FunctionType* funcTy){

  std::map<const llvm::Function *,FunctionWrapper *>::iterator FI =  FunctionWrapper::funcMap.begin();
  std::map<const llvm::Function *,FunctionWrapper *>::iterator FE =  FunctionWrapper::funcMap.end();

  for(; FI != FE; ++FI){
    if((*FI).first->getFunctionType() == funcTy && (*FI).first->getName() != "main"){
      errs() << (*FI).first->getName() << " function pointer! \n";

      //TODO:
      //color a ret node in callee(func ptr)randomly as long as we can combine them together with caller

    }
  }
}



void ProgramDependencyGraph::drawFormalParameterTree(Function* func, TreeType treeTy)
{
  for(list<ArgumentWrapper*>::iterator argI = FunctionWrapper::funcMap[func]->getArgWList().begin(),
	argE = FunctionWrapper::funcMap[func]->getArgWList().end(); argI != argE; ++argI){
    for(tree<InstructionWrapper*>::iterator TI = (*argI)->getTree(treeTy).begin(), 
	  TE = (*argI)->getTree(treeTy).end(); TI != TE; ++TI){	      		      
      for(int i = 0; i < TI.number_of_children(); i++){
	InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

	if(nullptr == *InstructionWrapper::nodes.find(*TI)) errs() << "DEBUG LINE 84 InstW NULL\n";
	if(nullptr == *InstructionWrapper::nodes.find(childW)) errs() << "DEBUG LINE 85 InstW NULL\n";

	PDG->addDependency(*InstructionWrapper::nodes.find(*TI), *InstructionWrapper::nodes.find(childW), PARAMETER);
      } 	      
    }//end for tree
  }//end for list
}

void ProgramDependencyGraph::drawActualParameterTree(CallInst* CI, TreeType treeTy)
{      
  for(list<ArgumentWrapper*>::iterator argI = CallWrapper::callMap[CI]->getArgWList().begin(),
	argE = CallWrapper::callMap[CI]->getArgWList().end(); argI != argE; ++argI){

    for(tree<InstructionWrapper*>::iterator TI = (*argI)->getTree(treeTy).begin(), 
	  TE = (*argI)->getTree(treeTy).end(); TI != TE; ++TI){	      		      
      for(int i = 0; i < TI.number_of_children(); i++){
	InstructionWrapper *childW = *(*argI)->getTree(treeTy).child(TI, i);

	if(nullptr == *InstructionWrapper::nodes.find(*TI)) errs() << "DEBUG LINE 103 InstW NULL\n";
	if(nullptr == *InstructionWrapper::nodes.find(childW)) errs() << "DEBUG LINE 104 InstW NULL\n";

	PDG->addDependency(*InstructionWrapper::nodes.find(*TI), *InstructionWrapper::nodes.find(childW), PARAMETER);
      } 	      
    }//end for tree
  }//end for list
}

void ProgramDependencyGraph::connectFunctionAndFormalTrees(llvm::Function *callee){

  //  errs() << "DEBUG 152: In connectFunctionAndFormalTrees, callee->getName() : " << callee->getName() << "\n";

  for(list<ArgumentWrapper*>::iterator argI = FunctionWrapper::funcMap[callee]->getArgWList().begin(),
	argE = FunctionWrapper::funcMap[callee]->getArgWList().end(); argI != argE; ++argI){
    InstructionWrapper *formal_inW = *(*argI)->getTree(FORMAL_IN_TREE).begin();
    InstructionWrapper *formal_outW = *(*argI)->getTree(FORMAL_OUT_TREE).begin();

    //connect Function's EntryNode with formal in/out tree roots 
    PDG->addDependency(FunctionWrapper::funcMap[callee]->getEntry(), *InstructionWrapper::nodes.find(formal_inW), PARAMETER);
    PDG->addDependency(FunctionWrapper::funcMap[callee]->getEntry(), *InstructionWrapper::nodes.find(formal_outW), PARAMETER);

    //TODO: connect corresponding instructions with internal formal tree nodes
  
    //two things: (1) formal-in --> callee's Store; (2) callee's Load --> formal-out
    for(tree<InstructionWrapper*>::iterator formal_in_TI= (*argI)->getTree(FORMAL_IN_TREE).begin(),
	  formal_in_TE  = (*argI)->getTree(FORMAL_IN_TREE).end(), 
	  formal_out_TI = (*argI)->getTree(FORMAL_OUT_TREE).begin();
	formal_in_TI != formal_in_TE; ++formal_in_TI, ++formal_out_TI){

      //connect formal-in and formal-out nodes formal-in --> formal-out
      PDG->addDependency(*InstructionWrapper::nodes.find(*formal_in_TI), *InstructionWrapper::nodes.find(*formal_out_TI), PARAMETER);
    
      //must handle nullptr case first
      if(nullptr == (*formal_in_TI)->getFieldType() ){break;}

      //connect formal-in-tree type nodes with storeinst in call_func, approximation used here
      if(nullptr != (*formal_in_TI)->getFieldType()){
	std::list<StoreInst*>::iterator SI = FunctionWrapper::funcMap[callee]->getStoreInstList().begin();
	std::list<StoreInst*>::iterator SE = FunctionWrapper::funcMap[callee]->getStoreInstList().end();
	for(;SI != SE; ++SI){
	  if((*formal_in_TI)->getFieldType() == (*SI)->getValueOperand()->getType()){
	    for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::nodes.begin();
		nodeIt != InstructionWrapper::nodes.end(); ++nodeIt){
	      if((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*SI))
		PDG->addDependency(*InstructionWrapper::nodes.find(*formal_in_TI), *InstructionWrapper::nodes.find(*nodeIt), DATA_GENERAL);	      
	    }}}
      }//end if nullptr == (*formal_in_TI)->getFieldType()

      //2. Callee's LoadInsts --> FORMAL_OUT in Callee function
      //must handle nullptr case first
      if(nullptr == (*formal_out_TI)->getFieldType() ){break;}

      if(nullptr != (*formal_out_TI)->getFieldType()){
	std::list<LoadInst*>::iterator LI = FunctionWrapper::funcMap[callee]->getLoadInstList().begin();
	std::list<LoadInst*>::iterator LE = FunctionWrapper::funcMap[callee]->getLoadInstList().end();
	for(;LI != LE; ++LI){
	  if((*formal_out_TI)->getFieldType() == (*LI)->getPointerOperand()->getType()->getContainedType(0)){
	    for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::nodes.begin();
		nodeIt != InstructionWrapper::nodes.end(); ++nodeIt){
	
	      if((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*LI)){		
		PDG->addDependency(*InstructionWrapper::nodes.find(*nodeIt), *InstructionWrapper::nodes.find(*formal_out_TI), DATA_GENERAL);
	      }}}}}
    }//end for (tree formal_in_TI...)
  }//end for arg iteration...
}


int ProgramDependencyGraph::connectCallerAndCallee(InstructionWrapper *InstW, llvm::Function *callee){

  if(InstW == nullptr || callee == nullptr){
    return 1;
  }
  
#if PARAMETER_TREE
  //callInst in caller --> Entry Node in callee
  PDG->addDependency(InstW, FunctionWrapper::funcMap[callee]->getEntry(), CONTROL);
#else
  PDG->addDependency(InstW, FunctionWrapper::funcMap[callee]->getEntry(), DATA_GENERAL);
#endif


  //ReturnInst in callee --> CallInst in caller
  for(list<Instruction*>::iterator RI = FunctionWrapper::funcMap[callee]->getReturnInstList().begin(), 
	RE = FunctionWrapper::funcMap[callee]->getReturnInstList().end(); RI != RE; ++RI){
  
    for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::nodes.begin();
	nodeIt != InstructionWrapper::nodes.end(); ++nodeIt){
	
      if((*nodeIt)->getInstruction() == *RI){
	if (nullptr != dyn_cast<ReturnInst>((*nodeIt)->getInstruction())->getReturnValue())
	  PDG->addDependency(*InstructionWrapper::nodes.find(*nodeIt), InstW, DATA_GENERAL);
	//TODO: indirect call, function name??
	else 
	  ;// errs() << "void ReturnInst: " << *(*nodeIt)->getInstruction() << " Function: " << callee_func->getName();
      }
    }
  }
#if PARAMETER_TREE

  //TODO: consider all kinds of connecting cases
  //connect caller InstW with ACTUAL IN/OUT parameter trees
  CallInst *CI = dyn_cast<CallInst>(InstW->getInstruction());

  for(list<ArgumentWrapper*>::iterator argI = CallWrapper::callMap[CI]->getArgWList().begin(),
	argE = CallWrapper::callMap[CI]->getArgWList().end(); argI != argE; ++argI){
    InstructionWrapper *actual_inW = *(*argI)->getTree(ACTUAL_IN_TREE).begin();
    InstructionWrapper *actual_outW = *(*argI)->getTree(ACTUAL_OUT_TREE).begin();
    if(nullptr == *InstructionWrapper::nodes.find(actual_inW)) errs() << "DEBUG LINE 168 InstW NULL\n";
    if(nullptr == *InstructionWrapper::nodes.find(actual_outW)) errs() << "DEBUG LINE 169 InstW NULL\n";
    PDG->addDependency(InstW, *InstructionWrapper::nodes.find(actual_inW), PARAMETER);
    PDG->addDependency(InstW, *InstructionWrapper::nodes.find(actual_outW), PARAMETER);    
  }

  //old way, process four trees at the same time, remove soon
  list<ArgumentWrapper*>::iterator formal_argI = FunctionWrapper::funcMap[callee]->getArgWList().begin();
  list<ArgumentWrapper*>::iterator formal_argE = FunctionWrapper::funcMap[callee]->getArgWList().end();
  list<ArgumentWrapper*>::iterator actual_argI = CallWrapper::callMap[CI]->getArgWList().begin();
  list<ArgumentWrapper*>::iterator actual_argE = CallWrapper::callMap[CI]->getArgWList().end();

  //increase formal/actual tree iterator simutaneously
  for(; formal_argI != formal_argE; ++formal_argI, ++actual_argI){
    //intra-connection between ACTUAL/FORMAL IN/OUT trees
    for(tree<InstructionWrapper*>::iterator actual_in_TI= (*actual_argI)->getTree(ACTUAL_IN_TREE).begin(),
	  actual_in_TE  = (*actual_argI)->getTree(ACTUAL_IN_TREE).end(), 
	  formal_in_TI  = (*formal_argI)->getTree(FORMAL_IN_TREE).begin(), //TI2
	  formal_out_TI = (*formal_argI)->getTree(FORMAL_OUT_TREE).begin(), //TI3
	  actual_out_TI = (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin();  //TI4
	actual_in_TI != actual_in_TE; ++actual_in_TI, ++formal_in_TI, ++formal_out_TI, ++actual_out_TI){
      //connect trees: antual-in --> formal-in, formal-out --> actual-out
      PDG->addDependency(*InstructionWrapper::nodes.find(*actual_in_TI), *InstructionWrapper::nodes.find(*formal_in_TI), PARAMETER);
      PDG->addDependency(*InstructionWrapper::nodes.find(*formal_out_TI), *InstructionWrapper::nodes.find(*actual_out_TI), PARAMETER);
    }//end for(tree...) intra-connection between actual/formal

    //TODO: why removing this debugging infor will cause segmentation fault?

    //3. ACTUAL_OUT --> LoadInsts in #Caller# function
    for(tree<InstructionWrapper*>::iterator actual_out_TI = (*actual_argI)->getTree(ACTUAL_OUT_TREE).begin(),
	  actual_out_TE = (*actual_argI)->getTree(ACTUAL_OUT_TREE).end(); actual_out_TI != actual_out_TE; ++actual_out_TI){
      //must handle nullptr case first
      if(nullptr == (*actual_out_TI)->getFieldType() ){break;}
      if(nullptr != (*actual_out_TI)->getFieldType()){
	std::list<LoadInst*>::iterator LI = FunctionWrapper::funcMap[InstW->getFunction()]->getLoadInstList().begin();
	std::list<LoadInst*>::iterator LE = FunctionWrapper::funcMap[InstW->getFunction()]->getLoadInstList().end();
	for(;LI != LE; ++LI){
	  if((*actual_out_TI)->getFieldType() == (*LI)->getType()){
	    for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::nodes.begin();
		nodeIt != InstructionWrapper::nodes.end(); ++nodeIt){
	      if((*nodeIt)->getInstruction() == dyn_cast<Instruction>(*LI))
		PDG->addDependency(*InstructionWrapper::nodes.find(*actual_out_TI), *InstructionWrapper::nodes.find(*nodeIt), DATA_GENERAL);
	    }}}}
    }// end for(tree actual_out_TI = getTree FORMAL_OUT_TREE)     
  }//end for argI iteration
  //end if PARAMETER_TREE
#endif
  return 0;
}


bool ProgramDependencyGraph::runOnModule(Module &M)
{
  Global_AA = &getAnalysis<AliasAnalysis>();
  errs() << "ProgramDependencyGraph::runOnModule" << '\n';
  FunctionWrapper::constructFuncMap(M);

  errs() << "funcMap size = " << FunctionWrapper::funcMap.size() << '\n';

  //Get the Module's list of global variable and function identifiers  
  errs()<<"======Global List: ======\n";

  for(llvm::Module::global_iterator globalIt = M.global_begin(); globalIt != M.global_end(); ++globalIt){
    //globalIt = M.getGlobalList().begin();      globalIt != M.getGlobalList().end(); ++globalIt){
    //    errs()<< "gloabal value: " << *globalIt << " alignment: " << (*globalIt).getAlignment() << "\n";
    InstructionWrapper *globalW = new InstructionWrapper(nullptr, nullptr, &(*globalIt), GLOBAL_VALUE);
    InstructionWrapper::nodes.insert(globalW);
    InstructionWrapper::globalList.insert(globalW);    
    //find all global pointer values and insert them into a list
    if(globalW->getValue()->getType()->getContainedType(0)->isPointerTy()){
      // errs() << " Pointer global value found! : " << *(globalW->getValue()) << "\n"; 
      gp_list.push_back(globalW);
    }
  }

  int funcs = 0;
  int colored_funcs = 0;
  int uncolored_funcs = 0;






  //process a module function by function
  for(Module::iterator F = M.begin(), E = M.end(); F != E; ++F)
    {
      if((*F).isDeclaration()){
	errs() << (*F).getName() << " is defined outside!" << "\n";
	continue;
      }
      
      funcs++;//label this author-defined function

      //initialize basicblock id bid for each BB
      errs() << "TEST F.getName = " << F->getName()<<"\n";
      for(BasicBlock &B : *F){
	errs() << "bid:"<<bid <<" "<< &B << " " << B << "-----\n";
	bbOrderMap[&B] = bid++;
      }
      errs() << "\n";

      //   removeBackedgesInFunction(*F);



      errs() << "PDG " << 1.0*funcs/M.getFunctionList().size()*100 << "% completed\n";

      InstructionWrapper::constructInstMap(*F);

      //find all Load/Store instructions for each F, insert to F's storeInstList and loadInstList
      for(inst_iterator I = inst_begin(F), IE = inst_end(F); I != IE; ++I){
	Instruction *pInstruction = dyn_cast<Instruction>(&*I);
	if(isa<StoreInst>(pInstruction)){
	  StoreInst* SI = dyn_cast<StoreInst>(pInstruction);
	  FunctionWrapper::funcMap[&*F]->getStoreInstList().push_back(SI);
	  FunctionWrapper::funcMap[&*F]->getPtrSet().insert(SI->getPointerOperand());
	  if(SI->getValueOperand()->getType()->isPointerTy()){
	    FunctionWrapper::funcMap[&*F]->getPtrSet().insert(SI->getValueOperand());
	  }
	} 
	if(isa<LoadInst>(pInstruction)){
	  LoadInst* LI = dyn_cast<LoadInst>(pInstruction);
	  FunctionWrapper::funcMap[&*F]->getLoadInstList().push_back(LI);
	  FunctionWrapper::funcMap[&*F]->getPtrSet().insert(LI->getPointerOperand());
	}
	if(isa<ReturnInst>(pInstruction))
	  FunctionWrapper::funcMap[&*F]->getReturnInstList().push_back(pInstruction);	
	if(isa<CallInst>(pInstruction))
	  FunctionWrapper::funcMap[&*F]->getCallInstList().push_back(dyn_cast<CallInst>(pInstruction));		  	      
	if(isa<AllocaInst>(pInstruction)){
	  AllocaInst* AI = dyn_cast<AllocaInst>(pInstruction);
	  if(AI->getName().find("retval") == string::npos)
	    FunctionWrapper::funcMap[&*F]->getAllocaMap()[pInstruction] = allocaMAX;
	}
      }
      //print PtrSet only

      DataDependencyGraph &ddgGraph = getAnalysis<DataDependencyGraph>(*F);
     
      ControlDependencyGraph &cdgGraph = getAnalysis<ControlDependencyGraph>(*F);
  
      cdgGraph.computeDependencies(*F, cdgGraph.PDT);

      //set Entries for Function, set up links between dummy entry nodes and their func*
      for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::funcInstWList[&*F].begin();
	  nodeIt != InstructionWrapper::funcInstWList[&*F].end(); ++nodeIt){
	InstructionWrapper *InstW = *nodeIt;
	if(InstW->getType() == ENTRY){
	  std::map<const llvm::Function *,FunctionWrapper *>::const_iterator FI = 
	    FunctionWrapper::funcMap.find(InstW->getFunction()); 
	  if(FI != FunctionWrapper::funcMap.end()){
	    //   errs() << "find successful!" << "\n";
	    FunctionWrapper::funcMap[InstW->getFunction()]->setEntry(InstW);
	  }   
	}
      }//end for set Entries...

      clock_t begin2 = clock();

      //the iteration should be done for the instMap, not original F
      for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::funcInstWList[&*F].begin();
	  nodeIt != InstructionWrapper::funcInstWList[&*F].end(); ++nodeIt)
	{
	  InstructionWrapper *InstW = *nodeIt;
	  Instruction *pInstruction = InstW->getInstruction();
	  //process call nodes, one call node will only be touched once(!InstW->getAccess)
	  if(pInstruction != nullptr && InstW->getType() == INST && isa<CallInst>(pInstruction) && !InstW->getAccess())
	    {
	      InstructionWrapper *CallInstW = InstW;
	      CallInst *CI = dyn_cast<CallInst>(pInstruction);
	      Function *callee = CI->getCalledFunction();
	      //if this is an indirect function invocation(function pointer, member function...)
	      // e.g.   %1 = load i32 (i32)** %p, align 8
	      //	%call = call i32 %1(i32 2))
	      if(callee == nullptr){
		Type* t = CI->getCalledValue()->getType();
		errs() << "indirect call, called Type t = " << *t << "\n";
		FunctionType* funcTy = cast<FunctionType>(cast<PointerType>(t)->getElementType());
		errs() << "afte cast<FunctionType>, ft = " << *funcTy <<"\n";
		connectAllPossibleFunctions(InstW, funcTy);
		continue;
	      }
	      //TODO: isIntrinsic or not? Consider intrinsics as common instructions for now, continue directly  
	      if(callee->isIntrinsic() || callee->isDeclaration()){
		//if it is a var_annotation, save the sensitive value by the way
		if(callee->getIntrinsicID() == Intrinsic::var_annotation){
		  Value* v = CI->getArgOperand(0);
		  errs() << "Intrinsic var_annotation: " << *v << "\n";
		  if(isa<BitCastInst>(v)){
		    Instruction *tempI = dyn_cast<Instruction>(v);
		    for(llvm::Use &U : tempI->operands()){
		      Value *v_for_cast = U.get();
		      sensitive_values.push_back(v_for_cast);
		    }}
		  else
		    sensitive_values.push_back(v);
		}		  
		continue;
	      }
	      //TODO: tail call processing
	      if(CI->isTailCall()){continue;}
	      //special cases done, common function
	      CallWrapper *callW = new CallWrapper(CI);
	      CallWrapper::callMap[CI] = callW;
	      //take recursive callInst as common callInst
	      if(0 == connectCallerAndCallee(InstW, callee)){
		InstW->setAccess(true);
	      }
	    }//end callnode

	  //second iteration on nodes to add both control and data Dependency
	  for(std::set<InstructionWrapper*>::iterator nodeIt2 = InstructionWrapper::funcInstWList[&*F].begin();
	      nodeIt2 != InstructionWrapper::funcInstWList[&*F].end(); ++nodeIt2){
	    InstructionWrapper *InstW2 = *nodeIt2;
  
	    //process all globals see whether dependency exists
	    if(InstW2->getType() == INST && isa<LoadInst>(InstW2->getInstruction())){

	      LoadInst* LI2 = dyn_cast<LoadInst>(InstW2->getInstruction());
	      
	      for(std::set<InstructionWrapper *>::const_iterator gi = InstructionWrapper::globalList.begin(); 
		  gi != InstructionWrapper::globalList.end(); ++gi){	   
		if(LI2->getPointerOperand() == (*gi)->getValue()){
		  PDG->addDependency(*gi, InstW2, GLOBAL_VALUE);
		}		
	      }// end searching globalList
	    }//end procesing load for global

	    if(InstW->getType() == INST){	       
	      if (ddgGraph.DDG->depends(InstW, InstW2)) {
		//only StoreInst->LoadInst edge can be annotated
		if(InstW2->getType() == INST 
		   && isa<StoreInst>(InstW->getInstruction())
		   && isa<LoadInst>(InstW2->getInstruction())){


		  PDG->addDependency(InstW, InstW2, DATA_RAW);
		  /*
		  //only back store can --> prev load
		  if (bbOrderMap[InstW->getInstruction()->getParent()] >= bbOrderMap[InstW2->getInstruction()->getParent()]){
		  errs() << InstW->getInstruction()->getParent() << " " << InstW2->getInstruction()->getParent() << "\n";
		  errs() << "bbOrderMap " << bbOrderMap[InstW->getInstruction()->getParent()] << "\n";  
		  errs() << "bbOrderMap " << bbOrderMap[InstW2->getInstruction()->getParent()] << "\n";  
		  errs() << "Store Loc:" << InstW->getInstruction()->getDebugLoc().getLine() << *InstW->getInstruction() << "\n";
		  errs() << "Load  Loc:" << InstW2->getInstruction()->getDebugLoc().getLine() << *InstW2->getInstruction() << "\n";

		  PDG->addDependency(InstW, InstW2, DATA_RAW);
		  }*/
		}
		else
		  PDG->addDependency(InstW, InstW2, DATA_DEF_USE);
	      }
	    
	      if(nullptr != InstW2->getInstruction()){		  
		if (cdgGraph.CDG->depends(InstW, InstW2)) {
		  PDG->addDependency(InstW, InstW2, CONTROL);
		}
	      }
	    }//end if(InstW->getType()== INST)

	    if(InstW->getType() == ENTRY){
	      if (nullptr != InstW2->getInstruction() && cdgGraph.CDG->depends(InstW, InstW2))
		PDG->addDependency(InstW, InstW2, CONTROL);
	    }	    
	  }//end second iteration for PDG->addDependency...
	} //end the iteration for finding CallInst     	
    }//end for(Module...

  //print PtrSet only
  //  #if 0

  errs() << "\n\n PDG construction completed! ^_^\n\n";
  errs() << "bid = " << bid << "\n";
  errs() << "funcs = " << funcs << "\n";
  errs() << "+++++++++++++++++++++++++++++++++++++++++++++\n";


  std::set<llvm::GlobalValue*> senGlobalSet;
  std::set<llvm::Instruction*> senInstSet;
  
  std::set<InstructionWrapper *>::const_iterator gi = InstructionWrapper::globalList.begin();
  std::set<InstructionWrapper *>::const_iterator ge = InstructionWrapper::globalList.end();

  errs() << "globalList.size = " << InstructionWrapper::globalList.size() << "\n";
  
  std::set<Function*> async_funcs;

  //sensitive global values(with annotations) can be directly get through Module.getNamedGlobal(GetNameGlobal in 3.9)
  auto global_annos = M.getNamedGlobal("llvm.global.annotations");
  if (global_annos){
    auto casted_array = cast<ConstantArray>(global_annos->getOperand(0));
    for (int i = 0; i < casted_array->getNumOperands(); i++) {
      auto casted_struct = cast<ConstantStruct>(casted_array->getOperand(i));

      if (auto sen_gv = dyn_cast<GlobalValue>(casted_struct->getOperand(0)->getOperand(0))) {
	auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(casted_struct->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();
	if (anno == "tainted") { 
	  errs() << "tainted global found! value = " << *sen_gv << "\n";
	  senGlobalSet.insert(sen_gv);
	}
      }

      //TODO: rewrite these code to make it work for our function annotation
      if (auto fn = dyn_cast<Function>(casted_struct->getOperand(0)->getOperand(0))) {
	auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(casted_struct->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();

	if (anno == "tainted") { 
	  async_funcs.insert(fn);
	  errs() << "async_funcs ++ tainted " << fn->getName() << "\n";
	}
      }// end if (auto...
    }// end for (int i ...
  }//end if (global_annos)




   
  //process all sensitive instructions in functions and all global values, color their corresponding nodes in set "nodes"   
  for(std::set<InstructionWrapper*>::iterator nodeIt = InstructionWrapper::nodes.begin();
      nodeIt != InstructionWrapper::nodes.end(); ++nodeIt){

    InstructionWrapper *InstW = *nodeIt;
    Instruction *pInstruction = InstW->getInstruction();

    //process annoatated sensitive values(actually are instructionWrappers) in functions
    for(int i = 0; i < sensitive_values.size(); i++){
      if(sensitive_values[i] == pInstruction){
	errs() << "sensitive_values " << i << " == "<< *pInstruction << "\n";
	sensitive_nodes.push_back(InstW); 
      }
    }

    //based on senGloabalSet, find annotated global InstructionWrappers
    if(InstW->getType() == GLOBAL_VALUE){
      GlobalValue *gv = dyn_cast<GlobalValue>(InstW->getValue());
	
      //if gv is sensitive(inside senGlobalSet)
      if(senGlobalSet.end() != senGlobalSet.find(gv)){
	errs() << "sensitive_global: " << *gv <<"\n";
	sensitive_nodes.push_back(InstW);

      }//end judging gv's sensitivity
    }//end global value
  }



  set<Function*> senFuncSet;
  //find all allocaInsts in each sensitive function.
  for(int i = 0; i < sensitive_values.size(); i++){
    Instruction* I = dyn_cast<Instruction>(sensitive_values[i]);
    //    initAllocaMap(*I->getParent()->getParent());

    //push into sensitive variable set
    taintMap[I].insert(I->getName());
    Function *senF = I->getParent()->getParent();
    //    allocaMap[I] = I->getDebugLoc().getLine(); //now this alloca is sensitive
    FunctionWrapper::funcMap[senF]->getAllocaMap()[I] = I->getDebugLoc().getLine();
    senFuncSet.insert(senF);
  }

  
  std::deque<const InstructionWrapper*> queue;
  for(int i = 0; i < sensitive_nodes.size(); i++){
    queue.push_back(sensitive_nodes[i]);
  }

  std::set<InstructionWrapper* > coloredInstSet;

  //worklist algorithm for propagation
  while(!queue.empty()){
    
    InstructionWrapper *InstW = const_cast<InstructionWrapper*>(queue.front());

    if(InstW->getType() == INST)
      FunctionWrapper::funcMap[InstW->getFunction()]->setVisited(true);

    queue.pop_front();
    //TODO: getInstruction should be removed  later, only for testing here temporarily

    //skip dummy nodes, call nodes
    if(InstW->getValue() == nullptr || isa<CallInst>(InstW->getValue())){
      ;
    }else {
      //encounter a non empty value
      coloredInstSet.insert(InstW);
    }
    
    DependencyNode<InstructionWrapper>* DNode = PDG->getNodeByData(InstW);
    std::set<string> TaintValuesOnInstW;
    bool isLoopBranch = false;

    //find taint value set on the current node
    if(taintMap.find(InstW->getInstruction()) != taintMap.end()){
      TaintValuesOnInstW = taintMap[InstW->getInstruction()];

      //print TaintValuesOnInstW
      errs() << InstW->getInstruction()->getDebugLoc().getLine()<<" TaintValuesOnInstW: ";
      for(auto const &c: TaintValuesOnInstW){
	errs() << c << " ";
      }
      errs() << "(" << *InstW->getInstruction() << ")\n";
    }


    //find while/for branchInsts first and mark, such kinds of br have self depends
    for(int i = 0; i < DNode->getDependencyList().size(); i++){
      if(nullptr != DNode->getDependencyList()[i].first->getData()){
	InstructionWrapper *adjacentInstW = 
	  *InstructionWrapper::nodes.find(const_cast<InstructionWrapper*>(DNode->getDependencyList()[i].first->getData())); 
	if(true == adjacentInstW->getFlag()){
	  if(isa<BranchInst>(adjacentInstW->getInstruction())){
	    if(adjacentInstW->getInstruction() == InstW->getInstruction()){
	      //	      errs() << "self depends: " << *InstW->getInstruction() << "\n";
	      isLoopBranch = true;
	    }}}}
    }//end for int

    for(int i = 0; i < DNode->getDependencyList().size(); i++){
      //we only do intra-procedural analysis, so skip call edges
      if(DNode->getDependencyList()[i].second == CALL){continue;}

      if(nullptr != DNode->getDependencyList()[i].first->getData()){
	InstructionWrapper *adjacentInstW = 
	  *InstructionWrapper::nodes.find(const_cast<InstructionWrapper*>(DNode->getDependencyList()[i].first->getData())); 
	Instruction* I = adjacentInstW->getInstruction();
	Function* F = I->getParent()->getParent();

	if(false == adjacentInstW->getFlag()){
	  //branchInst only generates control dependencies later, so no need to remove branch
	  queue.push_back(DNode->getDependencyList()[i].first->getData());
	  adjacentInstW->setFlag(true); //label the adjacent node visited
	  
	  //if InstW is sensitive, current I inherits the sensitivity 
	  if(coloredInstSet.find(InstW) != coloredInstSet.end()){
	    taintMap[I] = TaintValuesOnInstW;
	    errs() << "Enqueue coloredInstSet: " << *InstW->getInstruction() <<"\n"; 
	    coloredInstSet.insert(adjacentInstW);
	  }
	  //for each BranchInst(BB entry), scan the allocaMap to add those fixed taint alloca values
	  if(isa<BranchInst>(I)){
	    for(auto const& c: FunctionWrapper::funcMap[F]->getAllocaMap()){
	      if(c.second < I->getDebugLoc().getLine()){
		taintMap[I].insert(c.first->getName());
	      }
	    }


	    errs() << "DEBUG BR " << *I << " " << I->getDebugLoc().getLine() << "\n";
	    errs() << "taintMap: ";
	    for(auto const& c : taintMap[I]){
	      errs() << c << " "; 
	    }
	    errs() << "\n";
	  }


	  if(isa<StoreInst>(I)){
	    StoreInst* SI = dyn_cast<StoreInst>(I);
	    Value* p = SI->getPointerOperand();
	    if(isa<AllocaInst>(p)){
	      taintMap[I].insert(p->getName());
	      errs() << "in SI p =" <<*p << " SI=" <<*SI << "\n"; 
	      //if I's predecessor(InstW) is sensitive, and alloca is not sensitive yet 
	      if(coloredInstSet.find(InstW) != coloredInstSet.end()){
		errs() << "in SI predesessor " << *InstW->getInstruction() << "preLoc: " << InstW->getInstruction()->getDebugLoc().getLine()<<"\n";

		if(FunctionWrapper::funcMap[F]->getAllocaMap()[dyn_cast<Instruction>(p)] == allocaMAX){
		  FunctionWrapper::funcMap[F]->getAllocaMap()[dyn_cast<Instruction>(p)] = I->getDebugLoc().getLine();
		}
		errs() << "in SI getAllocaMap[] " << FunctionWrapper::funcMap[F]->getAllocaMap()[dyn_cast<Instruction>(p)] <<"\n";
		
	      }
	    }
	  }
	  else if(isa<LoadInst>(I)){
	    LoadInst* LI = dyn_cast<LoadInst>(I);
	    Value* p = LI->getPointerOperand();
	    if(isa<AllocaInst>(p) && FunctionWrapper::funcMap[F]->getAllocaMap()[dyn_cast<Instruction>(p)] < allocaMAX){
	      //      errs() << "DEBUG "<< I->getDebugLoc().getLine()<< " LI" << *LI << " p:" << *p << "\n"; 
	      taintMap[I].insert(p->getName());

	      //  errs() << "LI DEBUG:" << *LI <<" Loc:" << I->getDebugLoc().getLine() << "\n InstW(predecessor):" << *InstW->getInstruction() << "\n";

	      //	      if(coloredInstSet.find(InstW) != coloredInstSet.end()){
	      //	errs() << "predecessor is sensitive preLoc: " << InstW->getInstruction()->getDebugLoc().getLine() << "\n";
	      //}

	      if(isa<StoreInst>(InstW->getInstruction())){
		errs() << "SI->LI p: " << *p << " SI:" << *InstW->getInstruction() << " SILoc:" 
		       << InstW->getInstruction()->getDebugLoc().getLine() << "\n";
		
		FunctionWrapper::funcMap[F]->getAllocaMap()[dyn_cast<Instruction>(p)] = InstW->getInstruction()->getDebugLoc().getLine();
	      }
	    }
	  }//end isa<LoadInst>(I)

	  
	  else{//other Instructions like cmp
	    errs() << "other I: " << *I << "Loc: "<<I->getDebugLoc().getLine() << "\n";
	    errs() << "taintMap[I]: " ;
	    for(auto const& c : taintMap.find(I)->second){
	      errs() << c << " " ;
	    }
	    errs() << "\n";
	  }

	}//end if(false == adjacentInstW...)
      }//end if(nullptr != ...)
    }//end for int i = 0; i < DNode...
    //    errs() << "DEBUG 525" << "\n";
  }//end while(!queue...)

  for(auto const & F : senFuncSet){
    for (auto const &I : FunctionWrapper::funcMap[F]->getAllocaMap()){
      errs() << I.first << *I.first << " " << I.second << "\n";
    }
  }



  ofstream OS;
  OS.open("TaintValuesFile.txt");
  //  OS << "TEST OStream\n";

  for(auto const & F : senFuncSet){
    OS << "---------------------------------------------\n";
    OS << "function:"<< F->getName().str() <<"\n";
    OS << "---------------------------------------------\n";
    errs() << "--------------------------------------------\n";

    printTaintValues(*F, OS);
  }




  /*
    errs() << "\n\n++++++++++SENSITIVE INSTRUCTION List is as follows:++++++++++\n\n";
    int index = 0;
    for(auto const& senI : coloredInstSet){
    if(senI->getType() == INST){
    Instruction* I = senI->getInstruction();

    errs() << "SENSITIVE INSTRUCTION [" << index++ << "]" << " Value : " << *I << "\n";
    errs() << "Source Line Number: " << I->getDebugLoc().getLine() << "\n";

    if(taintMap.find(I) != taintMap.end()){
    //	errs() << taintMap.find(I)->second.size() << "\n";
    for(auto & pv : taintMap.find(I)->second){
    errs() << pv << " ";
    }
    errs()<< "\n";	
    }

    }
    } */   


  FunctionWrapper::funcMap.clear();
  CallWrapper::callMap.clear();
  InstructionWrapper::nodes.clear();
  InstructionWrapper::globalList.clear();
  InstructionWrapper::instMap.clear();
  InstructionWrapper::funcInstWList.clear();	


  //  allocaMap.clear();

  return false;

}

void ProgramDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequiredTransitive<AliasAnalysis>();
  AU.addRequired<ControlDependencyGraph>();
  AU.addRequired<DataDependencyGraph>();
  AU.setPreservesAll();
}


void ProgramDependencyGraph::print(llvm::raw_ostream &OS, const llvm::Module*) const{
  PDG->print(OS, getPassName());
}

ProgramDependencyGraph *CreateProgramDependencyGraphPass(){
  return new ProgramDependencyGraph();
}

static RegisterPass<cot::ProgramDependencyGraph> PDG("pdg", "Program Dependency Graph Construction", false, true);
