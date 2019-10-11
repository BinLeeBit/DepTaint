; ModuleID = 'if_1.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [8 x i8] c"tainted\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [9 x i8] c"if_1.cpp\00", section "llvm.metadata"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %flag = alloca i32, align 4
  %ch1 = alloca i32, align 4
  %ch2 = alloca i32, align 4
  %ch3 = alloca i32, align 4
  store i32 0, i32* %1
  call void @llvm.dbg.declare(metadata !{i32* %flag}, metadata !12), !dbg !13
  %2 = bitcast i32* %flag to i8*, !dbg !14
  call void @llvm.var.annotation(i8* %2, i8* getelementptr inbounds ([8 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8]* @.str1, i32 0, i32 0), i32 3), !dbg !14
  call void @llvm.dbg.declare(metadata !{i32* %ch1}, metadata !15), !dbg !16
  call void @llvm.dbg.declare(metadata !{i32* %ch2}, metadata !17), !dbg !18
  call void @llvm.dbg.declare(metadata !{i32* %ch3}, metadata !19), !dbg !20
  %3 = load i32* %flag, align 4, !dbg !21
  %4 = add nsw i32 %3, 2, !dbg !21
  store i32 %4, i32* %ch1, align 4, !dbg !21
  %5 = load i32* %flag, align 4, !dbg !22
  %6 = icmp ne i32 %5, 0, !dbg !22
  br i1 %6, label %7, label %9, !dbg !22

; <label>:7                                       ; preds = %0
  store i32 10, i32* %ch2, align 4, !dbg !24
  %8 = load i32* %flag, align 4, !dbg !26
  store i32 %8, i32* %ch3, align 4, !dbg !26
  br label %10, !dbg !27

; <label>:9                                       ; preds = %0
  store i32 11, i32* %ch2, align 4, !dbg !28
  br label %10

; <label>:10                                      ; preds = %9, %7
  ret i32 0, !dbg !30
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 4, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/rainbow/privateValueTracking/test/ex7/if_1.cpp] [DW_LANG_C_plus_plus]
!1 = metadata !{metadata !"if_1.cpp", metadata !"/home/rainbow/privateValueTracking/test/ex7"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 1, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 ()* @main, null, null, metadata !2, i32 1} ; [ DW_TAG_subprogram ] [line 1] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/rainbow/privateValueTracking/test/ex7/if_1.cpp]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!12 = metadata !{i32 786688, metadata !4, metadata !"flag", metadata !5, i32 3, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [flag] [line 3]
!13 = metadata !{i32 3, i32 44, metadata !4, null}
!14 = metadata !{i32 3, i32 3, metadata !4, null}
!15 = metadata !{i32 786688, metadata !4, metadata !"ch1", metadata !5, i32 4, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [ch1] [line 4]
!16 = metadata !{i32 4, i32 7, metadata !4, null}
!17 = metadata !{i32 786688, metadata !4, metadata !"ch2", metadata !5, i32 4, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [ch2] [line 4]
!18 = metadata !{i32 4, i32 11, metadata !4, null}
!19 = metadata !{i32 786688, metadata !4, metadata !"ch3", metadata !5, i32 4, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [ch3] [line 4]
!20 = metadata !{i32 4, i32 15, metadata !4, null}
!21 = metadata !{i32 5, i32 3, metadata !4, null}
!22 = metadata !{i32 6, i32 7, metadata !23, null}
!23 = metadata !{i32 786443, metadata !1, metadata !4, i32 6, i32 7, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/home/rainbow/privateValueTracking/test/ex7/if_1.cpp]
!24 = metadata !{i32 7, i32 5, metadata !25, null}
!25 = metadata !{i32 786443, metadata !1, metadata !23, i32 6, i32 12, i32 0, i32 1} ; [ DW_TAG_lexical_block ] [/home/rainbow/privateValueTracking/test/ex7/if_1.cpp]
!26 = metadata !{i32 8, i32 5, metadata !25, null} ; [ DW_TAG_imported_declaration ]
!27 = metadata !{i32 9, i32 3, metadata !25, null}
!28 = metadata !{i32 11, i32 5, metadata !29, null}
!29 = metadata !{i32 786443, metadata !1, metadata !23, i32 10, i32 8, i32 0, i32 2} ; [ DW_TAG_lexical_block ] [/home/rainbow/privateValueTracking/test/ex7/if_1.cpp]
!30 = metadata !{i32 14, i32 3, metadata !4, null}
