; ModuleID = 'integer.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [10 x i8] c"integer.c\00", section "llvm.metadata"
@.str2 = private unnamed_addr constant [20 x i8] c"please input size:\0A\00", align 1
@.str3 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str4 = private unnamed_addr constant [8 x i8] c"size:%d\00", align 1
@.str5 = private unnamed_addr constant [5 x i8] c"i:%d\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %i = alloca i32, align 4
  %buf = alloca [8 x i8], align 1
  %size = alloca i16, align 2
  %overflow = alloca [65550 x i8], align 16
  store i32 0, i32* %1
  store i32 %argc, i32* %2, align 4
  call void @llvm.dbg.declare(metadata !{i32* %2}, metadata !15), !dbg !16
  store i8** %argv, i8*** %3, align 8
  call void @llvm.dbg.declare(metadata !{i8*** %3}, metadata !17), !dbg !18
  call void @llvm.dbg.declare(metadata !{i32* %i}, metadata !19), !dbg !20
  call void @llvm.dbg.declare(metadata !{[8 x i8]* %buf}, metadata !21), !dbg !25
  %4 = bitcast [8 x i8]* %buf to i8*, !dbg !26
  call void @llvm.var.annotation(i8* %4, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([10 x i8]* @.str1, i32 0, i32 0), i32 6), !dbg !26
  call void @llvm.dbg.declare(metadata !{i16* %size}, metadata !27), !dbg !29
  %5 = bitcast i16* %size to i8*, !dbg !30
  call void @llvm.var.annotation(i8* %5, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([10 x i8]* @.str1, i32 0, i32 0), i32 7), !dbg !30
  call void @llvm.dbg.declare(metadata !{[65550 x i8]* %overflow}, metadata !31), !dbg !35
  %6 = bitcast [65550 x i8]* %overflow to i8*, !dbg !36
  call void @llvm.memset.p0i8.i64(i8* %6, i8 65, i64 65550, i32 16, i1 false), !dbg !36
  %7 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([20 x i8]* @.str2, i32 0, i32 0)), !dbg !37
  %8 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str3, i32 0, i32 0), i32* %i), !dbg !38
  %9 = load i32* %i, align 4, !dbg !39
  %10 = trunc i32 %9 to i16, !dbg !39
  store i16 %10, i16* %size, align 2, !dbg !39
  %11 = load i16* %size, align 2, !dbg !40
  %12 = zext i16 %11 to i32, !dbg !40
  %13 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @.str4, i32 0, i32 0), i32 %12), !dbg !40
  %14 = load i32* %i, align 4, !dbg !41
  %15 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str5, i32 0, i32 0), i32 %14), !dbg !41
  %16 = load i16* %size, align 2, !dbg !42
  %17 = zext i16 %16 to i32, !dbg !42
  %18 = icmp sgt i32 %17, 8, !dbg !42
  br i1 %18, label %19, label %20, !dbg !42

; <label>:19                                      ; preds = %0
  store i32 -1, i32* %1, !dbg !44
  br label %25, !dbg !44

; <label>:20                                      ; preds = %0
  %21 = bitcast [8 x i8]* %buf to i8*, !dbg !46
  %22 = bitcast [65550 x i8]* %overflow to i8*, !dbg !46
  %23 = load i32* %i, align 4, !dbg !46
  %24 = sext i32 %23 to i64, !dbg !46
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %21, i8* %22, i64 %24, i32 1, i1 false), !dbg !46
  store i32 0, i32* %1, !dbg !47
  br label %25, !dbg !47

; <label>:25                                      ; preds = %20, %19
  %26 = load i32* %1, !dbg !48
  ret i32 %26, !dbg !48
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) #2

declare i32 @printf(i8*, ...) #3

declare i32 @__isoc99_scanf(i8*, ...) #3

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #2

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!12, !13}
!llvm.ident = !{!14}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/integer-overflow/integer.c] [DW_LANG_C99]
!1 = metadata !{metadata !"integer.c", metadata !"/home/libbin/DepTaint/test/integer-overflow"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 4, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32, i8**)* @main, null, null, metadata !2, i32 4} ; [ DW_TAG_subprogram ] [line 4] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/integer-overflow/integer.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8, metadata !8, metadata !9}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, metadata !10} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from ]
!10 = metadata !{i32 786447, null, null, metadata !"", i32 0, i64 64, i64 64, i64 0, i32 0, metadata !11} ; [ DW_TAG_pointer_type ] [line 0, size 64, align 64, offset 0] [from char]
!11 = metadata !{i32 786468, null, null, metadata !"char", i32 0, i64 8, i64 8, i64 0, i32 0, i32 6} ; [ DW_TAG_base_type ] [char] [line 0, size 8, align 8, offset 0, enc DW_ATE_signed_char]
!12 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!13 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!14 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!15 = metadata !{i32 786689, metadata !4, metadata !"argc", metadata !5, i32 16777220, metadata !8, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [argc] [line 4]
!16 = metadata !{i32 4, i32 14, metadata !4, null}
!17 = metadata !{i32 786689, metadata !4, metadata !"argv", metadata !5, i32 33554436, metadata !9, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [argv] [line 4]
!18 = metadata !{i32 4, i32 26, metadata !4, null}
!19 = metadata !{i32 786688, metadata !4, metadata !"i", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [i] [line 5]
!20 = metadata !{i32 5, i32 9, metadata !4, null}
!21 = metadata !{i32 786688, metadata !4, metadata !"buf", metadata !5, i32 6, metadata !22, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [buf] [line 6]
!22 = metadata !{i32 786433, null, null, metadata !"", i32 0, i64 64, i64 8, i32 0, i32 0, metadata !11, metadata !23, i32 0, null, null, null} ; [ DW_TAG_array_type ] [line 0, size 64, align 8, offset 0] [from char]
!23 = metadata !{metadata !24}
!24 = metadata !{i32 786465, i64 0, i64 8}        ; [ DW_TAG_subrange_type ] [0, 7]
!25 = metadata !{i32 6, i32 49, metadata !4, null}
!26 = metadata !{i32 6, i32 5, metadata !4, null}
!27 = metadata !{i32 786688, metadata !4, metadata !"size", metadata !5, i32 7, metadata !28, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [size] [line 7]
!28 = metadata !{i32 786468, null, null, metadata !"unsigned short", i32 0, i64 16, i64 16, i64 0, i32 0, i32 7} ; [ DW_TAG_base_type ] [unsigned short] [line 0, size 16, align 16, offset 0, enc DW_ATE_unsigned]
!29 = metadata !{i32 7, i32 63, metadata !4, null}
!30 = metadata !{i32 7, i32 5, metadata !4, null}
!31 = metadata !{i32 786688, metadata !4, metadata !"overflow", metadata !5, i32 8, metadata !32, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [overflow] [line 8]
!32 = metadata !{i32 786433, null, null, metadata !"", i32 0, i64 524400, i64 8, i32 0, i32 0, metadata !11, metadata !33, i32 0, null, null, null} ; [ DW_TAG_array_type ] [line 0, size 524400, align 8, offset 0] [from char]
!33 = metadata !{metadata !34}
!34 = metadata !{i32 786465, i64 0, i64 65550}    ; [ DW_TAG_subrange_type ] [0, 65549]
!35 = metadata !{i32 8, i32 10, metadata !4, null} ; [ DW_TAG_imported_declaration ]
!36 = metadata !{i32 9, i32 5, metadata !4, null}
!37 = metadata !{i32 10, i32 5, metadata !4, null}
!38 = metadata !{i32 11, i32 5, metadata !4, null}
!39 = metadata !{i32 12, i32 5, metadata !4, null}
!40 = metadata !{i32 13, i32 5, metadata !4, null}
!41 = metadata !{i32 14, i32 5, metadata !4, null}
!42 = metadata !{i32 15, i32 8, metadata !43, null}
!43 = metadata !{i32 786443, metadata !1, metadata !4, i32 15, i32 8, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/integer-overflow/integer.c]
!44 = metadata !{i32 16, i32 9, metadata !45, null}
!45 = metadata !{i32 786443, metadata !1, metadata !43, i32 15, i32 17, i32 0, i32 1} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/integer-overflow/integer.c]
!46 = metadata !{i32 19, i32 5, metadata !4, null}
!47 = metadata !{i32 20, i32 5, metadata !4, null}
!48 = metadata !{i32 21, i32 1, metadata !4, null}
