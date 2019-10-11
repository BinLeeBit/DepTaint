/** ---*- C++ -*--- ProgramDependencies.h
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



#ifndef PROGRAMDEPENDENCIES_H
#define PROGRAMDEPENDENCIES_H

#include "llvm/Pass.h"
#include "DependencyGraph.h"
#include "FunctionWrapper.h"

#include <map>
#include <set>



namespace cot {

  typedef DependencyGraph<InstructionWrapper> ProgramDepGraph;

  /*!
   * Program Dependencies Graph
   */
  class ProgramDependencyGraph : public llvm::ModulePass
  {
  public:
    static char ID; // Pass ID, replacement for typeid
    ProgramDepGraph *PDG;
    static llvm::AliasAnalysis *Global_AA;

    ProgramDependencyGraph() : llvm::ModulePass(ID){
      PDG = new ProgramDepGraph();
    }

    ~ProgramDependencyGraph(){
      InstructionWrapper::releaseMemory();
      delete PDG;
    }
    void drawFormalParameterTree(Function* func, TreeType treeTy);

    void drawActualParameterTree(CallInst* CI, TreeType treeTy);

    //    void drawParameterTree(llvm::Function* call_func, TreeType treeTy);

    void connectAllPossibleFunctions(InstructionWrapper* CInstW, FunctionType* funcTy);

    void connectFunctionAndFormalTrees(Function *callee);
    int connectCallerAndCallee(InstructionWrapper *CInstW, llvm::Function *callee);
    //    int connectCallerAndCallee(CallInst *CI, llvm::Function *callee);

    bool runOnModule(llvm::Module &M);

    void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    const char *getPassName() const
    {
      return "Program Dependency Graph";
    }

    void print(llvm::raw_ostream &OS, const llvm::Module* M = 0) const;
  };


}

namespace llvm
{

  template <> struct GraphTraits<cot::ProgramDependencyGraph *>
    : public GraphTraits<cot::DepGraph*> {
    static NodeType *getEntryNode(cot::ProgramDependencyGraph *PG) {
      return *(PG->PDG->begin_children());
    }

    static nodes_iterator nodes_begin(cot::ProgramDependencyGraph *PG) {
      return PG->PDG->begin_children();
    }

    static nodes_iterator nodes_end(cot::ProgramDependencyGraph *PG) {
      return PG->PDG->end_children();
    }
  };

}


#endif // PROGRAMDEPENDENCIES_H
