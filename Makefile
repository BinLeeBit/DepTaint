
LLVM_CONFIG=/usr/local/bin/llvm-config
#CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0"
#LLVM_CONFIG=/home/lbb/Public/graduate/paper/Mine/DepTaint/llvm-3.5.0/build/bin/llvm-config

#LLVM_CONFIG=/usr/llvm-3.4.2/bin/llvm-config

#LLVM_CONFIG=/usr/llvm-3.3/bin/llvm-config

INC_DIR =./include  
SRC_DIR =./src
BIN_DIR =./bin
OBJ_DIR =./obj
ifneq ($(.bin), $(wildcard $(.bin)))
mkdir bin
endif

SRC = ${wildcard ${SRC_DIR}/*.cpp}
OBJ = ${patsubst %.cpp, $(OBJ_DIR)/%.o, ${notdir ${SRC}}}

TARGET=main
BIN_TARGET=${BIN_DIR}/${TARGET}

CXX=`$(LLVM_CONFIG) --bindir`/clang++
#CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -D__GLIBCXX_USE_CXX11_ABI=0 -fno-rtti -std=c++11 -I$(INC_DIR)
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti -g -O3 -Wno-deprecated -std=c++11 -I$(INC_DIR)
LDFLAGS=`$(LLVM_CONFIG) --ldflags` -fPIC

${BIN_TARGET} : ${OBJ}
	${CXX} -shared ${OBJ} ${LIB} -o ${BIN_DIR}/pdg.so $(LDFLAGS)
#guarantee the objs will automatically be rebuilt once the source files changed.
.PHONY : $(OBJS) 

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
#	echo "Compiling $< == S@"
	${CXX} -c ${CXXFLAGS} $< -o $@


clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/pdg.so


#all: pdg.so

#pdg.so: ConnectFunctions.o ProgramDependencies.o ControlDependencies.o DataDependencies.o DepPrinter.o FlowDependenceAnalysis.o
#	$(CXX) -shared ConnectFunctions.o ProgramDependencies.o ControlDependencies.o DataDependencies.o DepPrinter.o FlowDependenceAnalysis.o  ./DS-AA/lib/AssistDS/DSNodeEquivs.o ./DS-AA/lib/smack/SmackDSAAA.o ./DS-AA/lib/DSA/LLVMDataStructure.a ./DS-AA/lib/sos/DSAA.a -o pdg.so $(LDFLAGS) -fPIC

#ConnectFunctions.o: ConnectFunctions.cpp
#	$(CXX) -c ConnectFunctions.cpp -o ConnectFunctions.o $(CXXFLAGS)

#ProgramDependencies.o: ProgramDependencies.cpp
#	$(CXX) -c ProgramDependencies.cpp -o ProgramDependencies.o $(CXXFLAGS)

#ControlDependencies.o: ControlDependencies.cpp ControlDependencies.h
#	$(CXX) -c ControlDependencies.cpp -o ControlDependencies.o $(CXXFLAGS)

#DataDependencies.o: DataDependencies.cpp DataDependencies.h
#	$(CXX) -c DataDependencies.cpp -o DataDependencies.o $(CXXFLAGS)

#FlowDependenceAnalysis.o: FlowDependenceAnalysis.cpp FlowDependenceAnalysis.h  
#	$(CXX) -c FlowDependenceAnalysis.cpp -o FlowDependenceAnalysis.o $(CXXFLAGS)

#DepPrinter.o: DepPrinter.cpp
#	$(CXX) -c DepPrinter.cpp -o DepPrinter.o $(CXXFLAGS)


#mytest:
#	echo $(SRC)
#	echo $(OBJ)
#_DEPS = AllPasses.h ControlDependencies.h DependencyGraph.h FunctionWrapper.h SystemControlDependenceGraph.h \
	ConnectFunctions.h DataDependencies.h FlowDependenceAnalysis.h ProgramDependencies.h SystemDataDependencies.h
#DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

#$(patsubst <pattern>, <replacement>, <text>)
#search strings in <text>, if satisfies <pattern>, replace it with <replacement>
#_OBJ = ConnectFunctions.o ControlDependencies.o DataDependencies.o FlowDependenceAnalysis.o \
	DepPrinter.o ProgramDependencies.o
#OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))
