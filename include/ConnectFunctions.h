/** ---*- C++ -*--- ConnectFunctions.h
 * This header file defines some functions for connecting callers and callees, using parameter trees.
 */

#ifndef CONNECTFUNCTIONS_H_
#define CONNECTFUNCTIONS_H_


#include "llvm/Bitcode/ReaderWriter.h"

#include "FunctionWrapper.h"

#include "ProgramDependencies.h"

//#include "DependencyGraph.h"
//#include "SystemDataDependencies.h"
//#include "SystemControlDependenceGraph.h"

#include "AllPasses.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include <map>
#include <list>
#include <vector>
#include <iostream>

using namespace std;
using namespace cot;
using namespace llvm;


int buildFormalTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy);

int buildActualTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy, CallInst *CI);

void buildFormalTree(Argument *arg, TreeType treeTy);

void buildActualTree(CallInst* CI, Argument *arg, TreeType treeTy);

void buildFormalParameterTrees(Function* Func);

void buildActualParameterTrees(CallInst *CI);

#endif
