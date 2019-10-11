; ModuleID = 'stack.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [8 x i8] c"stack.c\00", section "llvm.metadata"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %first = alloca i8*, align 8
  %second = alloca i8*, align 8
  store i32 0, i32* %1
  store i32 %argc, i32* %2, align 4
  call void @llvm.dbg.declare(metadata !{i32* %2}, metadata !15), !dbg !16
  store i8** %argv, i8*** %3, align 8
  call void @llvm.dbg.declare(metadata !{i8*** %3}, metadata !17), !dbg !18
  call void @llvm.dbg.declare(metadata !{i8** %first}, metadata !19), !dbg !20
  %4 = bitcast i8** %first to i8*, !dbg !21
  call void @llvm.var.annotation(i8* %4, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8]* @.str1, i32 0, i32 0), i32 4), !dbg !21
  call void @llvm.dbg.declare(metadata !{i8** %second}, metadata !22), !dbg !23
  %5 = bitcast i8** %second to i8*, !dbg !24
  call void @llvm.var.annotation(i8* %5, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8]* @.str1, i32 0, i32 0), i32 5), !dbg !24
  %6 = call noalias i8* @malloc(i64 100) #2, !dbg !25
  store i8* %6, i8** %first, align 8, !dbg !25
  %7 = call noalias i8* @malloc(i64 12) #2, !dbg !26
  store i8* %7, i8** %second, align 8, !dbg !26
  %8 = load i32* %2, align 4, !dbg !27
  %9 = icmp sgt i32 %8, 1, !dbg !27
  br i1 %9, label %10, label %16, !dbg !27

; <label>:10                                      ; preds = %0
  %11 = load i8** %first, align 8, !dbg !29
  %12 = load i8*** %3, align 8, !dbg !29
  %13 = getelementptr inbounds i8** %12, i64 1, !dbg !29
  %14 = load i8** %13, align 8, !dbg !29
  %15 = call i8* @strcpy(i8* %11, i8* %14) #2, !dbg !29
  br label %16, !dbg !31

; <label>:16                                      ; preds = %10, %0
  %17 = load i8** %first, align 8, !dbg !32
  call void @free(i8* %17) #2, !dbg !32
  %18 = load i8** %second, align 8, !dbg !33
  call void @free(i8* %18) #2, !dbg !33
  ret i32 0, !dbg !34
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #3

; Function Attrs: nounwind
declare i8* @strcpy(i8*, i8*) #3

; Function Attrs: nounwind
declare void @free(i8*) #3

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!12, !13}
!llvm.ident = !{!14}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/stack-overflow/stack.c] [DW_LANG_C99]
!1 = metadata !{metadata !"stack.c", metadata !"/home/libbin/DepTaint/test/stack-overflow"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 3, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32, i8**)* @main, null, null, metadata !2, i32 3} ; [ DW_TAG_subprogram ] [line 3] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/stack-overflow/stack.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8, metadata !8, metadata !9}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, metadata !10} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from ]
!10 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, metadata !11} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from char]
!11 = metadata !{i32 786468, null, null, metadata !"char", i32 0, i64 8, i64 8, i64 0, i32 0, i32 6} ; [ DW_TAG_base_type ] [char] [line 0, size 8, align 8, offset 0, enc DW_ATE_signed_char]
!12 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!13 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!14 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!15 = metadata !{i32 786689, metadata !4, metadata !"argc", metadata !5, i32 16777219, metadata !8, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [argc] [line 3]
!16 = metadata !{i32 3, i32 14, metadata !4, null}
!17 = metadata !{i32 786689, metadata !4, metadata !"argv", metadata !5, i32 33554435, metadata !9, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [argv] [line 3]
!18 = metadata !{i32 3, i32 25, metadata !4, null}
!19 = metadata !{i32 786688, metadata !4, metadata !"first", metadata !5, i32 4, metadata !10, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [first] [line 4]
!20 = metadata !{i32 4, i32 50, metadata !4, null}
!21 = metadata !{i32 4, i32 5, metadata !4, null}
!22 = metadata !{i32 786688, metadata !4, metadata !"second", metadata !5, i32 5, metadata !10, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [second] [line 5]
!23 = metadata !{i32 5, i32 50, metadata !4, null}
!24 = metadata !{i32 5, i32 5, metadata !4, null}
!25 = metadata !{i32 6, i32 13, metadata !4, null}
!26 = metadata !{i32 7, i32 14, metadata !4, null}
!27 = metadata !{i32 8, i32 8, metadata !28, null} ; [ DW_TAG_imported_declaration ]
!28 = metadata !{i32 786443, metadata !1, metadata !4, i32 8, i32 8, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/stack-overflow/stack.c]
!29 = metadata !{i32 9, i32 9, metadata !30, null}
!30 = metadata !{i32 786443, metadata !1, metadata !28, i32 8, i32 15, i32 0, i32 1} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/stack-overflow/stack.c]
!31 = metadata !{i32 10, i32 5, metadata !30, null}
!32 = metadata !{i32 11, i32 5, metadata !4, null}
!33 = metadata !{i32 12, i32 5, metadata !4, null}
!34 = metadata !{i32 13, i32 5, metadata !4, null}
