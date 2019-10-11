
#include "ControlDependencies.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/Interval.h"
#include "llvm/Support/Debug.h"
#include <string>



#include "llvm/IR/PassManager.h"
#include "llvm/IR/CallingConv.h"
//#include "llvm/Analysis/Verifier.h"
//#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/IR/IRBuilder.h"

#include "FunctionWrapper.h"


using namespace cot;
using namespace llvm;


static IRBuilder<> Builder(getGlobalContext());

char ControlDependencyGraph::ID = 0;

int ControlDependencyGraph::entry_id = 0; 

std::set<InstructionWrapper *> InstructionWrapper::nodes;

std::set<InstructionWrapper *> InstructionWrapper::globalList;

std::map<const llvm::Instruction *,InstructionWrapper *> InstructionWrapper::instMap;

std::map<const llvm::Function*, std::set<InstructionWrapper*> > InstructionWrapper::funcInstWList;




int ControlDependencyGraph::getDependenceType(const BasicBlock *A, const BasicBlock *B) const {
  assert(A && B);
  if (const llvm::BranchInst *b = dyn_cast<BranchInst>(A->getTerminator())) {
    if (b->isConditional()) {
      if (b->getSuccessor(0) == B) {
	return ControlType::TRUE;
      } else if (b->getSuccessor(1) == B) {
	return ControlType::FALSE;
      } else {
	errs() << *A << "\n" << *B << "\n";
	assert(false && "Asking for edge type between unconnected basic blocks!");
      }
    }
  }
  return ControlType::OTHER;
}

void ControlDependencyGraph::computeDependencies(llvm::Function &F, llvm::PostDominatorTree *PDT) {
  /*
   * The EdgeSet should always contains the Start->EntryNode
   * edge. This will lead to add every node in the path from the
   * ExitNode(the immediate postdom of Start) and the EntryNode as
   * control dependent on Start.
   */
  /// Zhiyuan: explicitly construct the dummy ENTRY NODE:
  //llvm::errs() << "ControlDependencies.cpp - setRootFor " << F.getName().str() << "\n";
  root = new InstructionWrapper(&F, ENTRY);

  //  errs() << "$$$$$$$$$$$$$$$ computeDependencies $$$$$$$$$$$$$$$$" << '\n';
  //errs() << "CDG before insert nodes.size :" << InstructionWrapper::nodes.size() << " Function: " << F.getName().str() << '\n';
  InstructionWrapper::nodes.insert(root);
  InstructionWrapper::funcInstWList[&F].insert(root);

  errs() << " CDG.cpp after insert nodes.size " << InstructionWrapper::nodes.size() << " Function: " << F.getName().str() << '\n';
  //TODO: maybe need to use nodes set element explicitly instead of using root directly
  FunctionWrapper::funcMap[&F]->setEntry(root);


  DomTreeNode *node = PDT->getNode(&F.getEntryBlock());

  while (node && node->getBlock())
    {
      /*
       * Walking the path backward and adding dependencies.
       */
      addDependency(root, node->getBlock(), CONTROL);

      node = node->getIDom(); //const DomTreeNodeBase<NodeT> *IDom;
    }

  std::vector<std::pair<BasicBlock *, BasicBlock *> > EdgeSet;

  for (Function::iterator I = F.begin(), E = F.end(); I != E; ++I)
    {
      ///Zhiyuan comment: find adjacent BasicBlock pairs in CFG, but the predecessor does not dominate successor.
      for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI)
	{
	  assert(I && *SI);
	  if (!PDT->dominates(*SI, I)) {

	    //          errs() << I->getName() << "\n";
	    //          errs() << "I:   " << I << "\n";
	    //          errs() << "*SI: " << (*SI)->getName() << "\n";
	    //          errs() << "Added to (A, B) edge set.\n";
	    EdgeSet.push_back(std::make_pair(I, *SI));
	  }
	}
    }

  typedef std::vector<std::pair<BasicBlock *, BasicBlock *> >::iterator EdgeItr;

  ///find nearest common ancestor in Post Dominator Tree for the BasicBlock pair.

  errs() << "computerDependencies DEBUG 1\n";

  for (EdgeItr I = EdgeSet.begin(), E = EdgeSet.end(); I != E; ++I)
    {
      std::pair<BasicBlock *, BasicBlock *> Edge = *I;
      BasicBlock *L = PDT->findNearestCommonDominator(Edge.first, Edge.second);
      int type = getDependenceType(Edge.first, Edge.second);
      //      BasicBlockWrapper *A = BasicBlockWrapper::bbMap[Edge.first];

      // capture loop dependence
      if (L == Edge.first) {
	//	errs() << "\t find A == L: " << L->getName() << "\n";

	// TODO: commented by shen Jan 28th 2018 for robert's project
	//	addDependency(Edge.first, L, type);
	//     errs() << "DepType: " << type << "\n";
      }

      //        errs() << "computerDependencies DEBUG 2\n";

      //      DomTreeNode *domNode = PDT[Edge.second];
      DomTreeNode *domNode = PDT->getNode(Edge.second);

      // errs()<< "domNode = " << domNode << "\n";
      //errs()<< "domNode->getBlock() = " << domNode->getBlock() << "\n";
      if(domNode == nullptr){
	errs() << "domNode is null!\n";
	continue;
      }


      while (domNode->getBlock() != L)
	{
	  //TODO: added by shen
	  //CDG->addDependency(Edge.first, domNode->getBlock(), CONTROL);
	  addDependency(Edge.first, domNode->getBlock(), type);
	  domNode = domNode->getIDom(); 
	}
    }
  //    errs() << "computerDependencies DEBUG 3\n";
}

/// added 2 functions by Zhiyuan: Mar 4, 2015. transfer basic blocks to instructions
void ControlDependencyGraph::addDependency(InstructionWrapper* from, llvm::BasicBlock* to, int type) {
  for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii) {
    if (llvm::Instruction* Ins = llvm::dyn_cast<llvm::Instruction>(ii) ) {
      InstructionWrapper *iw = InstructionWrapper::instMap[Ins];
      
      //disconnect icmpInst and its branchInst, guarantee no loops
      //      if(nullptr != from->getInstruction() && isa<BranchInst>(from->getInstruction()) && isa<CmpInst>(iw->getInstruction()))
      
	CDG->addDependency(from, iw, type);
    }
  }
}

void ControlDependencyGraph::addDependency(llvm::BasicBlock* from, llvm::BasicBlock* to, int type) {
  Instruction *Ins = from->getTerminator();
  assert(Ins);
  InstructionWrapper *iw = InstructionWrapper::instMap[Ins];
  //self loop
  if (from == to) {

    /// Zhiyuan: Bug Fix 04/23/2015: in a "while condition" basic block, all the instructions
    /// before the br inst including the branch inst control depend on the br inst.
    for (llvm::BasicBlock::iterator ii = from->begin(), ie = from->end(); ii != ie; ++ii) {
      InstructionWrapper *iwTo = InstructionWrapper::instMap[ii];
      CDG->addDependency(iw, iwTo, type);
    }
  } else {
    for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii) {
      InstructionWrapper *iwTo = InstructionWrapper::instMap[ii];
      CDG->addDependency(iw, iwTo, type);
    }
  }
}

bool ControlDependencyGraph::runOnFunction(Function &F)
{
  InstructionWrapper::constructInstMap(F);
  //  PostDominatorTree &PDT = getAnalysis<PostDominatorTree>();
  PDT = &getAnalysis<PostDominatorTree>();
  //computeDependencies(F, PDT);
  return false;
}


void ControlDependencyGraph::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
  AU.addRequired<PostDominatorTree>();
}


void ControlDependencyGraph::print(raw_ostream &OS, const Module*) const
{
  CDG->print(OS, getPassName());
}


ControlDependencyGraph *CreateControlDependencyGraphPass()
{
  return new ControlDependencyGraph();
}

/*
  INITIALIZE_PASS(ControlDependencyGraph, "cdg",
  "Control Dependency Graph Construction",
  true,
  true)
*/
static RegisterPass<cot::ControlDependencyGraph> CDG("cdg", "Control Dependency Graph Construction", false, true);


      //Dummy node
      /*
      LLVMContext &Context = getGlobalContext();

      std::string str = std::to_string(ControlDependencyGraph::entry_id);

      AllocaInst* entryDummyInst = new AllocaInst(llvm::Type::getInt32Ty(Context), 0, str.c_str());

      entryDummyInst->setAlignment(4);
      //          Value* V = dyn_cast<Value>(node);
      // entryDummyInst->setOperand(0,V);

      //   errs() << "@@@@@@@ entryDummyInst = " << *entryDummyInst << '\n'; 
      ControlDependencyGraph::entry_id++;
      //      errs() << "@@@@@@@ entry_id = " << ControlDependencyGraph::entry_id << '\n'; 
      
      Instruction *DummyInst = dyn_cast<Instruction>(entryDummyInst);
      

      errs() << "@@@@@@@ DummyInst = " << *DummyInst << '\n';

      if (InstructionWrapper::instMap.find(entryDummyInst) == InstructionWrapper::instMap.end()) //if not in instMap yet, insert 
	{
	  InstructionWrapper *iw = new InstructionWrapper(DummyInst, &F, ENTRY);

	  InstructionWrapper::instMap[DummyInst] = iw;
	  errs() << "@@@@@@@ instMap.size = " << InstructionWrapper::instMap.size() << '\n'; 
	  //TODO add dependency in CDG
	  //CDG->addDependency(iw, , type);
	}

      //clear LLVMContext references to avoid dangling pointers
      entryDummyInst->dropAllReferences();
      */

      
