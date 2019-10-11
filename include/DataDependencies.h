/** ---*- C++ -*--- DataDependencies.h
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



#ifndef DATADEPENDENCIES_H
#define DATADEPENDENCIES_H

#include "llvm/Pass.h"
#include "DependencyGraph.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "FlowDependenceAnalysis.h"

static
llvm::AliasAnalysis::ModRefResult GetLocation(const llvm::Instruction *Inst,
                                        llvm::AliasAnalysis::Location &Loc,
                                        llvm::AliasAnalysis *AA);



namespace cot
{
  typedef DependencyGraph<InstructionWrapper> DataDepGraph;
  /*!
   * Data Dependency Graph
   */

  static std::vector <InstructionWrapper*> gp_list;

  class DataDependencyGraph : public llvm::FunctionPass
  {
  public:
    static char ID; // Pass ID, replacement for typeid
    DataDepGraph *DDG;

    DataDependencyGraph() : FunctionPass(ID)
    {
      DDG = new DataDepGraph();
    }

    ~DataDependencyGraph()
    {
      InstructionWrapper::releaseMemory();
      delete DDG;
    }

    virtual bool runOnFunction(llvm::Function &F);

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    virtual const char *getPassName() const
    {
      return "Data Dependency Graph";
    }

    virtual void print(llvm::raw_ostream &OS, const llvm::Module* M = 0) const;
  };
}

namespace llvm
{

  template <> struct GraphTraits<cot::DataDependencyGraph *>
      : public GraphTraits<cot::DepGraph*> {
    static NodeType *getEntryNode(cot::DataDependencyGraph *DG) {
      return *(DG->DDG->begin_children());
    }

    static nodes_iterator nodes_begin(cot::DataDependencyGraph *DG) {
      return DG->DDG->begin_children();
    }

    static nodes_iterator nodes_end(cot::DataDependencyGraph *DG) {
      return DG->DDG->end_children();
    }
  };

}

#endif // DATADEPENDENCIES_H
