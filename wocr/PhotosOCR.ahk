#Requires AutoHotkey v2.0

if A_Args.Length = 0
{
    MsgBox "Usage: PhotosOCR imagefile"
    ExitApp
}

imageFile := A_Args[1]

Loop Files, imageFile
{
    imageFile := A_LoopFileFullPath
    break
}

if !FileExist(imageFile)
{
    MsgBox "File not found:`n" imageFile
    ExitApp
}

Run imageFile

if !WinWaitActive("ahk_exe Photos.exe", , 10)
{
    MsgBox "Photos window not found"
    ExitApp
}

Sleep 3000

; Make sure window is in a known position
WinMove 0, 0, 1200, 900, "ahk_exe Photos.exe"

Sleep 1000

; ---- Click Scan text button ----
; These coordinates need to be adjusted once
Click 620, 850

; Wait for OCR processing
Sleep 3000

; ---- Copy text ----
A_Clipboard := ""

Send "^a"
Sleep 300
Send "^c"

if !ClipWait(5)
{
    MsgBox "No text copied"
    ExitApp
}

text := A_Clipboard

SplitPath imageFile, , &dir, &ext, &name
outputFile := dir "\" name ".ocr.txt"
;outputFile := A_Desktop "\test.txt"

;MsgBox outputFile

if FileExist(outputFile)
    FileDelete outputFile

FileEncoding "UTF-8"
FileAppend text, outputFile

WinClose "ahk_exe Photos.exe"

;MsgBox "Saved:`n" outputFile
