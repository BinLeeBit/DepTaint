/** ---*- C++ -*--- SystemDataDependencies.h
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



#ifndef SYSTEMDATADEPENDENCIES_H
#define SYSTEMDATADEPENDENCIES_H

#include "llvm/Pass.h"
#include "DependencyGraph.h"
#include "DataDependencies.h"

using namespace llvm;

namespace cot
{
  typedef DependencyGraph<InstructionWrapper> DataDepGraph;

  /*!
   * Data Dependency Graph
   */
  class SystemDataDependencyGraph : public llvm::ModulePass
  {
  public:
    static char ID; // Pass ID, replacement for typeid
    DataDepGraph *SystemDG;
    //static std::set<BasicBlockWrapper*> ExtraNodes;

    SystemDataDependencyGraph() : ModulePass(ID)
    {
      SystemDG = NULL;
      //      DDG = new DataDepGraph();
      //      errs()<< "SystemDataDependencyGraph()" << '\n';
 
    }

    ~SystemDataDependencyGraph()
    {
      //      BasicBlockWrapper::releaseMemory();
      //    delete DDG;

    }

    virtual bool runOnModule(llvm::Module &M);

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    virtual const char *getPassName() const
    {
      return "SystemData Dependency Graph";
    }

    virtual void print(llvm::raw_ostream &OS, const llvm::Module* M = 0) const;
  };
}

namespace llvm
{

  template <> struct GraphTraits<cot::SystemDataDependencyGraph *>
      : public GraphTraits<cot::DepGraph*> {
    static NodeType *getEntryNode(cot::SystemDataDependencyGraph *DG) {
      return *(DG->SystemDG->begin_children());
    }

    static nodes_iterator nodes_begin(cot::SystemDataDependencyGraph *DG) {
      return DG->SystemDG->begin_children();
    }

    static nodes_iterator nodes_end(cot::SystemDataDependencyGraph *DG) {
      return DG->SystemDG->end_children();
    }
  };

}

#endif // DATADEPENDENCIES_H
