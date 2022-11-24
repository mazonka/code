Attribute VB_Name = "NewMacros"
Sub cpp_form()
Attribute cpp_form.VB_Description = "Macro recorded 03/12/02 by omazonka"
Attribute cpp_form.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.cppformat"
'
' cppformat Macro
' Macro recorded 03/12/02 by omazonka
'
    Selection.WholeStory
    Selection.Font.Name = "Courier"
    Selection.Font.Size = 6
    Selection.HomeKey Unit:=wdStory
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Underline = wdUnderlineSingle
    Selection.Find.Replacement.Font.Bold = True
    With Selection.Find
        .Text = "private"
        .Replacement.Text = "private"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Underline = wdUnderlineSingle
    Selection.Find.Replacement.Font.Bold = True
    With Selection.Find
        .Text = "public"
        .Replacement.Text = "public"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Underline = wdUnderlineSingle
    Selection.Find.Replacement.Font.Bold = True
    With Selection.Find
        .Text = "protected"
        .Replacement.Text = "protected"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Italic = True
    With Selection.Find
        .Text = "void"
        .Replacement.Text = "void"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "int"
    Selection.Find.Replacement.Text = "int"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "unsigned"
    Selection.Find.Replacement.Text = "unsigned"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "static"
    Selection.Find.Replacement.Text = "static"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "long"
    Selection.Find.Replacement.Text = "long"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "byte"
    Selection.Find.Replacement.Text = "byte"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "bool"
    Selection.Find.Replacement.Text = "bool"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "true"
    Selection.Find.Replacement.Text = "true"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "flase"
    Selection.Find.Replacement.Text = "false"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Italic = True
    With Selection.Find
        .Text = "ulong"
        .Replacement.Text = "ulong"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    Selection.Find.Replacement.Font.Bold = True
    With Selection.Find
        .Text = "class"
        .Replacement.Text = "class"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    With Selection.Find
        .Text = "struct"
        .Replacement.Text = "struct"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    With Selection.Find
        .Text = "union"
        .Replacement.Text = "union"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "#include"
    Selection.Find.Replacement.Text = "#include"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "#define"
    Selection.Find.Replacement.Text = "#define"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "#ifdef"
    Selection.Find.Replacement.Text = "#ifdef"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "#ifndef"
    Selection.Find.Replacement.Text = "#ifndef"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "#endif"
    Selection.Find.Replacement.Text = "#endif"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "typedef"
    Selection.Find.Replacement.Text = "typedef"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "template"
    Selection.Find.Replacement.Text = "template"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.ClearFormatting
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find.Replacement.Font
        .Italic = True
        .Bold = True
 '       .ColorIndex = wdRed
    End With
    With Selection.Find
        .Text = "if"
        .Replacement.Text = "if"
        .Forward = True
        .Wrap = wdFindAsk
        .Format = True
        .MatchCase = True
        .MatchWholeWord = True
        .MatchWildcards = False
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "else"
    Selection.Find.Replacement.Text = "else"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "return"
    Selection.Find.Replacement.Text = "return"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "switch"
    Selection.Find.Replacement.Text = "switch"
    Selection.Find.Execute Replace:=wdReplaceAll
    
    Selection.Find.Text = "new"
    Selection.Find.Replacement.Text = "new"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "delete"
    Selection.Find.Replacement.Text = "delete"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "operator"
    Selection.Find.Replacement.Text = "operator"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "case"
    Selection.Find.Replacement.Text = "case"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "for"
    Selection.Find.Replacement.Text = "for"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "while"
    Selection.Find.Replacement.Text = "while"
    Selection.Find.Execute Replace:=wdReplaceAll

    Selection.Find.Text = "do"
    Selection.Find.Replacement.Text = "do"
    Selection.Find.Execute Replace:=wdReplaceAll
'
' cpp2 Macro
' Macro recorded 03/12/02 by omazonka
'
    Selection.Find.ClearFormatting
    With Selection.Find.Font
        .Bold = False
        .Italic = False
    End With
    Selection.Find.Replacement.ClearFormatting
    With Selection.Find.Replacement.Font
        .Bold = True
'        .ColorIndex = wdBlue
    End With
    With Selection.Find
        .Text = "[a-zA-Z]@::<*>"
        .Replacement.Text = "^&"
        .Forward = True
        .Wrap = wdFindContinue
        .Format = True
        .MatchCase = False
        .MatchWholeWord = False
        .MatchWildcards = True
        .MatchSoundsLike = False
        .MatchAllWordForms = False
    End With
    Selection.Find.Execute Replace:=wdReplaceAll
End Sub
Sub cpp_page()
Attribute cpp_page.VB_Description = "Macro recorded 03/12/02 by omazonka"
Attribute cpp_page.VB_ProcData.VB_Invoke_Func = "Normal.NewMacros.cpp_page"
'
' cpp_page Macro
' Macro recorded 03/12/02 by omazonka
'
    With ActiveDocument.PageSetup
        .LineNumbering.Active = False
        .Orientation = wdOrientPortrait
        .TopMargin = InchesToPoints(1)
        .BottomMargin = InchesToPoints(1)
        .LeftMargin = InchesToPoints(1)
        .RightMargin = InchesToPoints(1)
        .Gutter = InchesToPoints(0)
        .HeaderDistance = InchesToPoints(0.5)
        .FooterDistance = InchesToPoints(0.5)
        .PageWidth = InchesToPoints(8.5)
        .PageHeight = InchesToPoints(11)
        .FirstPageTray = wdPrinterDefaultBin
        .OtherPagesTray = wdPrinterDefaultBin
        .SectionStart = wdSectionNewPage
        .OddAndEvenPagesHeaderFooter = False
        .DifferentFirstPageHeaderFooter = False
        .VerticalAlignment = wdAlignVerticalTop
        .SuppressEndnotes = False
        .MirrorMargins = False
    End With
    If ActiveWindow.View.SplitSpecial <> wdPaneNone Then
        ActiveWindow.Panes(2).Close
    End If
    If ActiveWindow.ActivePane.View.Type = wdNormalView Or ActiveWindow. _
        ActivePane.View.Type = wdOutlineView Or ActiveWindow.ActivePane.View.Type _
         = wdMasterView Then
        ActiveWindow.ActivePane.View.Type = wdPageView
    End If
    With ActiveDocument.PageSetup.TextColumns
        .SetCount NumColumns:=2
        .EvenlySpaced = True
        .LineBetween = False
        .Width = InchesToPoints(3.15)
        .Spacing = InchesToPoints(0.2)
    End With
End Sub
