; ModuleID = 'heap.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [7 x i8] c"heap.c\00", section "llvm.metadata"
@.str2 = private unnamed_addr constant [33 x i8] c"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %str = alloca i8*, align 8
  %dst = alloca [10 x i8], align 1
  store i32 0, i32* %1
  call void @llvm.dbg.declare(metadata !{i8** %str}, metadata !12), !dbg !15
  %2 = bitcast i8** %str to i8*, !dbg !16
  call void @llvm.var.annotation(i8* %2, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([7 x i8]* @.str1, i32 0, i32 0), i32 5), !dbg !16
  store i8* getelementptr inbounds ([33 x i8]* @.str2, i32 0, i32 0), i8** %str, align 8, !dbg !16
  call void @llvm.dbg.declare(metadata !{[10 x i8]* %dst}, metadata !17), !dbg !21
  %3 = getelementptr inbounds [10 x i8]* %dst, i32 0, i32 0, !dbg !22
  %4 = load i8** %str, align 8, !dbg !22
  %5 = call i8* @strcpy(i8* %3, i8* %4) #2, !dbg !22
  ret i32 0, !dbg !23
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

; Function Attrs: nounwind
declare i8* @strcpy(i8*, i8*) #3

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/heap-overflow/heap.c] [DW_LANG_C99]
!1 = metadata !{metadata !"heap.c", metadata !"/home/libbin/DepTaint/test/heap-overflow"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 4, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 0, i1 false, i32 ()* @main, null, null, metadata !2, i32 4} ; [ DW_TAG_subprogram ] [line 4] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/heap-overflow/heap.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!12 = metadata !{i32 786688, metadata !4, metadata !"str", metadata !5, i32 5, metadata !13, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [str] [line 5]
!13 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, metadata !14} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from char]
!14 = metadata !{i32 786468, null, null, metadata !"char", i32 0, i64 8, i64 8, i64 0, i32 0, i32 6} ; [ DW_TAG_base_type ] [char] [line 0, size 8, align 8, offset 0, enc DW_ATE_signed_char]
!15 = metadata !{i32 5, i32 51, metadata !4, null}
!16 = metadata !{i32 5, i32 5, metadata !4, null}
!17 = metadata !{i32 786688, metadata !4, metadata !"dst", metadata !5, i32 6, metadata !18, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [dst] [line 6]
!18 = metadata !{i32 786433, null, null, metadata !"", i32 0, i64 80, i64 8, i32 0, i32 0, metadata !14, metadata !19, i32 0, null, null, null} ; [ DW_TAG_array_type ] [line 0, size 80, align 8, offset 0] [from char]
!19 = metadata !{metadata !20}
!20 = metadata !{i32 786465, i64 0, i64 10}       ; [ DW_TAG_subrange_type ] [0, 9]
!21 = metadata !{i32 6, i32 10, metadata !4, null}
!22 = metadata !{i32 7, i32 5, metadata !4, null}
!23 = metadata !{i32 8, i32 5, metadata !4, null} ; [ DW_TAG_imported_declaration ]
