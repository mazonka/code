
; File:UInt128x64.asm
; build obj-file with
; ml64 /nologo /c /Zf /Fo$(IntDir)UInt128x64.obj UInt128x64.asm

.CODE

;void uint128div(_uint128 &dst, const _uint128 &x, const _uint128 &y);
uint128div PROC
    push        rbx
    push        rsi
    push        rcx
    mov         r9, rdx
    mov         rax, qword ptr[r8+8]
    or          rax, rax
    jne         L1
    mov         rcx, qword ptr[r8]
    mov         rax, qword ptr[r9+8]
    xor         rdx, rdx
    div         rcx
    mov         rbx, rax
    mov         rax, qword ptr[r9]
    div         rcx
    mov         rdx, rbx
    jmp         L2
L1:
    mov         rcx, rax
    mov         rbx, qword ptr[r8]
    mov         rdx, qword ptr[r9+8]
    mov         rax, qword ptr[r9]
L3:
    shr         rcx, 1
    rcr         rbx, 1
    shr         rdx, 1
    rcr         rax, 1
    or          rcx, rcx
    jne         L3
    div         rbx
    mov         rsi, rax
    mul         qword ptr[r8+8]
    mov         rcx, rax
    mov         rax, qword ptr[r8]
    mul         rsi
    add         rdx, rcx
    jb          L4
    cmp         rdx, qword ptr[r9+8]
    ja          L4
    jb          L5
    cmp         rax, qword ptr[r9]
    jbe         L5
L4:
    dec         rsi
L5:
    xor         rdx, rdx
    mov         rax, rsi
L2:
    pop         rcx
    pop         rsi
    pop         rbx
    mov         qword ptr[rcx], rax
    mov         qword ptr[rcx+8], rdx
    ret
uint128div ENDP

;void uint128rem(_uint128 &dst, const _uint128 &x, const _uint128 &y);
uint128rem PROC
    push        rbx
    push        rcx
    mov         r9, rdx
    mov         rax, qword ptr[r8+8]
    or          rax, rax
    jne         L1
    mov         rcx, qword ptr[r8]
    mov         rax, qword ptr[r9+8]
    xor         rdx, rdx
    div         rcx
    mov         rax, qword ptr[r9]
    div         rcx
    mov         rax, rdx
    xor         rdx, rdx
    jmp         L2
L1:
    mov         rcx, rax
    mov         rbx, qword ptr[r8]
    mov         rdx, qword ptr[r9+8]
    mov         rax, qword ptr[r9]
L3:
    shr         rcx, 1
    rcr         rbx, 1
    shr         rdx, 1
    rcr         rax, 1
    or          rcx, rcx
    jne         L3
    div         rbx
    mov         rcx, rax
    mul         qword ptr[r8+8]
    xchg        rax, rcx
    mul         qword ptr[r8]
    add         rdx, rcx
    jb          L4
    cmp         rdx, qword ptr[r9+8]
    ja          L4
    jb          L5
    cmp         rax, qword ptr[r9]
    jbe         L5
L4:
    sub         rax, qword ptr[r8]
    sbb         rdx, qword ptr[r8+8]
L5:
    sub         rax, qword ptr[r9]
    sbb         rdx, qword ptr[r9+8]
    neg         rdx
    neg         rax
    sbb         rdx, 0
L2:
    pop         rcx
    pop         rbx
    mov         qword ptr[rcx], rax
    mov         qword ptr[rcx+8], rdx
    ret
uint128rem ENDP

;int uint128cmp(const _uint128 &n1, const _uint128 &n2);
uint128cmp PROC
    mov         rax, qword ptr[rcx+8]       ; n1.hi
    cmp         rax, qword ptr[rdx+8]       ; n2.hi
    jb          lessthan                    ; usigned compare of n1.hi and n2.hi
    ja          greaterthan
    mov         rax, qword ptr[rcx]         ; n2.lo
    cmp         rax, qword ptr[rdx]         ; n2.lo
    jb          lessthan                    ; unsigned compare of n1.lo and n2.lo
    ja          greaterthan
    mov         rax, 0                      ; they are equal
    ret
greaterthan:
    mov         rax, 1
    ret
lessthan:
    mov         rax, -1
    ret
uint128cmp ENDP

END
