# DepTaint
## 1. Install
* Version of [LLVM-3.5.0](http://releases.llvm.org/download.html#3.5.0): If you have this version, you can skip installation of LLVM. We need to download Clang `source code (.sig)`, `LLVM source code (.sig)`, `Compiler RT source code (.sig)`. Then UnZip `LLVM source code` and rename `clang`, UnZip `Compiler RT source code`. `make` and `make install` need more than one hour.
```
# cp -rf clang llvm-3.5.0/tools
# cp -rf compiler-rt-3.5.0.src llvm-3.5.0/projects
# cd llvm-3.5.0
# mkdir build
# cd build
# cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" ../
# make -j 4
# sudo make install
```

* Install Deptaint
```
# git clone https://github.com/BinLeeBit/DepTaint
# cd DepTaint
# mkdir bin obj
# sudo make
```

## 2. Run (select the test: [ex6](test/ex6))
* Translate `ex6.c` to the Intermediate Representation(IR)
```
# clang -emit-llvm -c -g ex6.c -o ex6.bc
```

* (Optional) Translate binary IR(.bc) to readable type(.ll)
```
# llvm-dis ex6.bc
```

* Use Deptaint to analyze IR. You can get a program dependence gragph `dot` file and a taint variable `txt` file.
```
# opt -load ./bin/pdg.so -dot-pdg ex6.bc
```