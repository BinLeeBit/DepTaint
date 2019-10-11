/** ---*- C++ -*--- ControlDependencies.h
 *
 * Copyright (C) 2015 soslab
 *
 */



#ifndef CONTROLDEPENDENCIES_H
#define CONTROLDEPENDENCIES_H


#include "llvm/Analysis/PostDominators.h"
#include "DependencyGraph.h"
//#include "llvm/Pass.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/raw_ostream.h"

namespace cot
{
  typedef DependencyGraph<InstructionWrapper> ControlDepGraph;

  /*!
   * Control Dependency Graph
   */
  class ControlDependencyGraph : public llvm::FunctionPass
  {
  public:
    static char ID; // Pass ID, replacement for typeid
    ControlDepGraph *CDG;
    llvm::PostDominatorTree *PDT;
    
    ControlDependencyGraph() : llvm::FunctionPass(ID)
    {
      isLibrary = false;
      CDG = new ControlDepGraph();
    }

    ~ControlDependencyGraph()
    {
      InstructionWrapper::releaseMemory();
      delete CDG;
    }

    bool runOnFunction(llvm::Function &F);

    void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    const char *getPassName() const
    {
      return "Control Dependency Graph";
    }

    void print(llvm::raw_ostream &OS, const llvm::Module* M = 0) const;

    InstructionWrapper* getRoot() const {
      return root;
    }

    int getDependenceType(const llvm::BasicBlock *AW, const llvm::BasicBlock *BW) const;

    bool isLibraryCall() {
      return isLibrary;
    }
    /// Zhiyuan: this is for library call
    void mockLibraryCall(llvm::Function &F) {
      llvm::errs() << "ControlDependencies.h - setRootFor " << F.getName().str() << "\n";
      root = new InstructionWrapper(&F, ENTRY);
      isLibrary = true;
      //revised by shen May 9, 2015
      //         InstructionWrapper::nodes.insert(root);
    }
    void computeDependencies(llvm::Function &F, llvm::PostDominatorTree *PDT);

  private:
    /// added by Zhiyuan: Mar 4, 2015. transfer basic blocks to instructions
    void addDependency(InstructionWrapper* from, llvm::BasicBlock* to, int type);
    void addDependency(llvm::BasicBlock* from, llvm::BasicBlock* to, int type);

    /// added by Zhiyuan: Feb 19, 2015.
    llvm::Function *func;
    InstructionWrapper *root;
    bool isLibrary;
    static int entry_id; 

  };
}

namespace llvm
{

  template <> struct GraphTraits<cot::ControlDependencyGraph *>
    : public GraphTraits<cot::DepGraph*> {
    static NodeType *getEntryNode(cot::ControlDependencyGraph *CG) {
      return *(CG->CDG->begin_children());
    }

    static nodes_iterator nodes_begin(cot::ControlDependencyGraph *CG) {
      return CG->CDG->begin_children();
    }

    static nodes_iterator nodes_end(cot::ControlDependencyGraph *CG) {
      return CG->CDG->end_children();
    }
  };

}


#endif // CONTROLDEPENDENCIES_H
