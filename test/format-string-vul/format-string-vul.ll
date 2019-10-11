; ModuleID = 'format-string-vul.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [20 x i8] c"format-string-vul.c\00", section "llvm.metadata"
@.str2 = private unnamed_addr constant [11 x i8] c"AAAAAAA%n\0A\00", align 1
@.str3 = private unnamed_addr constant [9 x i8] c"BBBBBBB\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %flag = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %1
  call void @llvm.dbg.declare(metadata !{i32* %flag}, metadata !12), !dbg !13
  store i32 0, i32* %flag, align 4, !dbg !14
  call void @llvm.dbg.declare(metadata !{i32* %a}, metadata !15), !dbg !16
  %2 = bitcast i32* %a to i8*, !dbg !17
  call void @llvm.var.annotation(i8* %2, i8* getelementptr inbounds ([10 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([20 x i8]* @.str1, i32 0, i32 0), i32 5), !dbg !17
  store i32 0, i32* %a, align 4, !dbg !17
  br label %3, !dbg !18

; <label>:3                                       ; preds = %13, %0
  %4 = load i32* %a, align 4, !dbg !19
  %5 = icmp slt i32 %4, 5, !dbg !19
  br i1 %5, label %6, label %14, !dbg !19

; <label>:6                                       ; preds = %3
  %7 = load i32* %flag, align 4, !dbg !21
  %8 = icmp ne i32 %7, 0, !dbg !21
  br i1 %8, label %9, label %11, !dbg !21

; <label>:9                                       ; preds = %6
  %10 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str2, i32 0, i32 0), i32* %a), !dbg !24
  store i32 0, i32* %flag, align 4, !dbg !26
  br label %13, !dbg !27

; <label>:11                                      ; preds = %6
  %12 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([9 x i8]* @.str3, i32 0, i32 0)), !dbg !28
  store i32 1, i32* %flag, align 4, !dbg !30
  br label %13

; <label>:13                                      ; preds = %11, %9
  br label %3, !dbg !31

; <label>:14                                      ; preds = %3
  %15 = load i32* %a, align 4, !dbg !32
  %16 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str4, i32 0, i32 0), i32 %15), !dbg !32
  ret i32 0, !dbg !33
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #2

declare i32 @printf(i8*, ...) #3

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }
attributes #3 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5.0 (tags/RELEASE_350/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !"", i32 1} ; [ DW_TAG_compile_unit ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c] [DW_LANG_C99]
!1 = metadata !{metadata !"format-string-vul.c", metadata !"/home/libbin/DepTaint/test/format-string-vul"}
!2 = metadata !{}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 2, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 ()* @main, null, null, metadata !2, i32 3} ; [ DW_TAG_subprogram ] [line 2] [def] [scope 3] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 2, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!12 = metadata !{i32 786688, metadata !4, metadata !"flag", metadata !5, i32 4, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [flag] [line 4]
!13 = metadata !{i32 4, i32 9, metadata !4, null}
!14 = metadata !{i32 4, i32 5, metadata !4, null}
!15 = metadata !{i32 786688, metadata !4, metadata !"a", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [a] [line 5]
!16 = metadata !{i32 5, i32 49, metadata !4, null}
!17 = metadata !{i32 5, i32 5, metadata !4, null}
!18 = metadata !{i32 6, i32 5, metadata !4, null}
!19 = metadata !{i32 6, i32 5, metadata !20, null}
!20 = metadata !{i32 786443, metadata !1, metadata !4, i32 6, i32 5, i32 1, i32 4} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!21 = metadata !{i32 7, i32 13, metadata !22, null}
!22 = metadata !{i32 786443, metadata !1, metadata !23, i32 7, i32 13, i32 0, i32 1} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!23 = metadata !{i32 786443, metadata !1, metadata !4, i32 6, i32 17, i32 0, i32 0} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!24 = metadata !{i32 8, i32 13, metadata !25, null} ; [ DW_TAG_imported_declaration ]
!25 = metadata !{i32 786443, metadata !1, metadata !22, i32 7, i32 18, i32 0, i32 2} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!26 = metadata !{i32 9, i32 13, metadata !25, null}
!27 = metadata !{i32 10, i32 9, metadata !25, null}
!28 = metadata !{i32 12, i32 13, metadata !29, null}
!29 = metadata !{i32 786443, metadata !1, metadata !22, i32 11, i32 13, i32 0, i32 3} ; [ DW_TAG_lexical_block ] [/home/libbin/DepTaint/test/format-string-vul/format-string-vul.c]
!30 = metadata !{i32 13, i32 13, metadata !29, null}
!31 = metadata !{i32 15, i32 5, metadata !23, null}
!32 = metadata !{i32 16, i32 5, metadata !4, null}
!33 = metadata !{i32 17, i32 5, metadata !4, null}
