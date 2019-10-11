; ModuleID = 'double-free.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [14 x i8] c"double-free.c\00", section "llvm.metadata"
@.str2 = private unnamed_addr constant [11 x i8] c"malloc:%p\0A\00", align 1
@.str3 = private unnamed_addr constant [9 x i8] c"free p1\0A\00", align 1
@.str4 = private unnamed_addr constant [9 x i8] c"free p3\0A\00", align 1
@.str5 = private unnamed_addr constant [9 x i8] c"free p2\0A\00", align 1
@.str6 = private unnamed_addr constant [13 x i8] c"Double free\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i8**, align 8
  %p1 = alloca i8*, align 8
  %p2 = alloca i8*, align 8
  %p3 = alloca i8*, align 8
  store i32 %argc, i32* %1, align 4
  call void @llvm.dbg.declare(metadata !{i32* %1}, metadata !15), !dbg !16
  store i8** %argv, i8*** %2, align 8
  call void @llvm.dbg.declare(metadata !{i8*** %2}, metadata !17), !dbg !18
  call void @llvm.dbg.declare(metadata !{i8** %p1}, metadata !19), !dbg !21
  %3 = bitcast i8** %p1 to i8*, !dbg !22
  call void @llvm.var.annotation(i8* %3, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([14 x i8]* @.str1, i32 0, i32 0), i32 4), !dbg !22
  call void @llvm.dbg.declare(metadata !{i8** %p2}, metadata !23), !dbg !24
  %4 = bitcast i8** %p2 to i8*, !dbg !25
  call void @llvm.var.annotation(i8* %4, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([14 x i8]* @.str1, i32 0, i32 0), i32 5), !dbg !25
  call void @llvm.dbg.declare(metadata !{i8** %p3}, metadata !26), !dbg !27
  %5 = bitcast i8** %p3 to i8*, !dbg !28
  call void @llvm.var.annotation(i8* %5, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([14 x i8]* @.str1, i32 0, i32 0), i32 6), !dbg !28
  %6 = call noalias i8* @malloc(i64 100) #2, !dbg !29
  store i8* %6, i8** %p1, align 8, !dbg !29
  %7 = load i8** %p1, align 8, !dbg !30
  %8 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0), i8* %7), !dbg !30
  %9 = call noalias i8* @malloc(i64 100) #2, !dbg !31
  store i8* %9, i8** %p2, align 8, !dbg !31
  %10 = load i8** %p2, align 8, !dbg !32
  %11 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0), i8* %10), !dbg !32
  %12 = call noalias i8* @malloc(i64 100) #2, !dbg !33
  store i8* %12, i8** %p3, align 8, !dbg !33
  %13 = load i8** %p3, align 8, !dbg !34
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0), i8* %13), !dbg !34
  %15 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([9 x i8]* @.str3, i32 0, i32 0)), !dbg !35
  %16 = load i8** %p1, align 8, !dbg !36
  call void @free(i8* %16) #2, !dbg !36
  %17 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([9 x i8]* @.str4, i32 0, i32 0)), !dbg !37
  %18 = load i8** %p3, align 8, !dbg !38
  call void @free(i8* %18) #2, !dbg !38
  %19 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([9 x i8]* @.str5, i32 0, i32 0)), !dbg !39
  %20 = load i8** %p2, align 8, !dbg !40
  call void @free(i8* %20) #2, !dbg !40
  %21 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([13 x i8]* @.str6, i32 0, i32 0)), !dbg !41
  %22 = load i8** %p2, align 8, !dbg !42
  call void @free(i8* %22) #2, !dbg !42
  ret i32 0, !dbg !43
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #3

declare i32 @printf(i8*, ...) #4

; Function Attrs: nounwind
declare void @free(i8*) #3

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!12, !13}
!llvm.ident = !{!14}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/double-free/double-free.c] [DW_LANG_C99]
!1 = metadata !{metadata !"double-free.c", metadata !"/home/libbin/DepTaint/test/double-free"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 3, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32, i8**)* @main, null, null, metadata !2, i32 3} ; [ DW_TAG_subprogram ] [line 3] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/double-free/double-free.c]
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
!18 = metadata !{i32 3, i32 26, metadata !4, null}
!19 = metadata !{i32 786688, metadata !4, metadata !"p1", metadata !5, i32 4, metadata !20, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [p1] [line 4]
!20 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, null} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from ]
!21 = metadata !{i32 4, i32 51, metadata !4, null}
!22 = metadata !{i32 4, i32 5, metadata !4, null}
!23 = metadata !{i32 786688, metadata !4, metadata !"p2", metadata !5, i32 5, metadata !20, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [p2] [line 5]
!24 = metadata !{i32 5, i32 51, metadata !4, null}
!25 = metadata !{i32 5, i32 5, metadata !4, null}
!26 = metadata !{i32 786688, metadata !4, metadata !"p3", metadata !5, i32 6, metadata !20, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [p3] [line 6]
!27 = metadata !{i32 6, i32 51, metadata !4, null}
!28 = metadata !{i32 6, i32 5, metadata !4, null}
!29 = metadata !{i32 8, i32 8, metadata !4, null} ; [ DW_TAG_imported_declaration ]
!30 = metadata !{i32 9, i32 5, metadata !4, null}
!31 = metadata !{i32 10, i32 8, metadata !4, null}
!32 = metadata !{i32 11, i32 5, metadata !4, null}
!33 = metadata !{i32 12, i32 8, metadata !4, null}
!34 = metadata !{i32 13, i32 5, metadata !4, null}
!35 = metadata !{i32 15, i32 5, metadata !4, null}
!36 = metadata !{i32 16, i32 5, metadata !4, null}
!37 = metadata !{i32 17, i32 5, metadata !4, null}
!38 = metadata !{i32 18, i32 5, metadata !4, null}
!39 = metadata !{i32 19, i32 5, metadata !4, null}
!40 = metadata !{i32 20, i32 5, metadata !4, null}
!41 = metadata !{i32 22, i32 5, metadata !4, null}
!42 = metadata !{i32 23, i32 5, metadata !4, null}
!43 = metadata !{i32 24, i32 1, metadata !4, null}
