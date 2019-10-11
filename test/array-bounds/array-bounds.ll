; ModuleID = 'array-bounds.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [15 x i8] c"array-bounds.c\00", section "llvm.metadata"
@main.array = private unnamed_addr constant [3 x i32] [i32 111, i32 222, i32 333], align 4
@.str2 = private unnamed_addr constant [21 x i8] c"please input index:\0A\00", align 1
@.str3 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str4 = private unnamed_addr constant [14 x i8] c"array[%d]=%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %index = alloca i32, align 4
  %array = alloca [3 x i32], align 4
  store i32 0, i32* %1
  call void @llvm.dbg.declare(metadata !{i32* %index}, metadata !12), !dbg !13
  %2 = bitcast i32* %index to i8*, !dbg !14
  call void @llvm.var.annotation(i8* %2, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([15 x i8]* @.str1, i32 0, i32 0), i32 3), !dbg !14
  call void @llvm.dbg.declare(metadata !{[3 x i32]* %array}, metadata !15), !dbg !19
  %3 = bitcast [3 x i32]* %array to i8*, !dbg !20
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast ([3 x i32]* @main.array to i8*), i64 12, i32 4, i1 false), !dbg !20
  %4 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([21 x i8]* @.str2, i32 0, i32 0)), !dbg !21
  %5 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str3, i32 0, i32 0), i32* %index), !dbg !22
  %6 = load i32* %index, align 4, !dbg !23
  %7 = load i32* %index, align 4, !dbg !23
  %8 = sext i32 %7 to i64, !dbg !23
  %9 = getelementptr inbounds [3 x i32]* %array, i32 0, i64 %8, !dbg !23
  %10 = load i32* %9, align 4, !dbg !23
  %11 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([14 x i8]* @.str4, i32 0, i32 0), i32 %6, i32 %10), !dbg !23
  %12 = load i32* %index, align 4, !dbg !24
  %13 = sext i32 %12 to i64, !dbg !24
  %14 = getelementptr inbounds [3 x i32]* %array, i32 0, i64 %13, !dbg !24
  store i32 233, i32* %14, align 4, !dbg !24
  ret i32 0, !dbg !25
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #2

declare i32 @printf(i8*, ...) #3

declare i32 @__isoc99_scanf(i8*, ...) #3

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/array-bounds/array-bounds.c] [DW_LANG_C99]
!1 = metadata !{metadata !"array-bounds.c", metadata !"/home/libbin/DepTaint/test/array-bounds"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 2, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 0, i1 false, i32 ()* @main, null, null, metadata !2, i32 2} ; [ DW_TAG_subprogram ] [line 2] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/array-bounds/array-bounds.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!12 = metadata !{i32 786688, metadata !4, metadata !"index", metadata !5, i32 3, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [index] [line 3]
!13 = metadata !{i32 3, i32 48, metadata !4, null}
!14 = metadata !{i32 3, i32 5, metadata !4, null}
!15 = metadata !{i32 786688, metadata !4, metadata !"array", metadata !5, i32 4, metadata !16, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [array] [line 4]
!16 = metadata !{i32 786433, null, null, metadata !"", i32 0, i64 96, i64 32, i32 0, i32 0, metadata !8, metadata !17, i32 0, null, null, null} ; [ DW_TAG_array_type ] [line 0, size 96, align 32, offset 0] [from int]
!17 = metadata !{metadata !18}
!18 = metadata !{i32 786465, i64 0, i64 3}        ; [ DW_TAG_subrange_type ] [0, 2]
!19 = metadata !{i32 4, i32 9, metadata !4, null}
!20 = metadata !{i32 4, i32 5, metadata !4, null}
!21 = metadata !{i32 5, i32 5, metadata !4, null}
!22 = metadata !{i32 6, i32 5, metadata !4, null}
!23 = metadata !{i32 7, i32 5, metadata !4, null}
!24 = metadata !{i32 9, i32 5, metadata !4, null}
!25 = metadata !{i32 10, i32 5, metadata !4, null}
