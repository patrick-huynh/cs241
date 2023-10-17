; start prologue
.import init
.import new
.import delete
.import print

lis $4 ; $4 will always hold 4
.word 4
lis $10 ; $10 will always hold address for print
.word print
lis $11 ; $1 will always hold 1
.word 1
sub $29, $30, $4 ; setup frame pointer

; calling init
sw $31, -4($30)
sub $30, $30, $4
lis $31
.word init
jalr $31
add $30, $30, $4
lw $31, -4($30)

; end prologue

sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; while loop
loop1:
; comparison
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, endWhile1
; printing
sw $1, -4($30)
sub $30, $30, $4
; binary op
; plus op: ptr + int
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
; lvalue statement
; assignment through id
; binary op
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, loop1
endWhile1:
; lvalue statement
; assignment through id
; calling procedure
; saving registers
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; binary op
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
; restoring registers
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, -8($29)
; lvalue statement
; assignment through id
lis $3
.word 0
sw $3, -12($29)
; while loop
loop2:
; comparison
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, endWhile2
; printing
sw $1, -4($30)
sub $30, $30, $4
; binary op
; plus op: ptr + int
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -12($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $1, -4($30)
; lvalue statement
; assignment through id
; binary op
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -12($29)
beq $0, $0, loop2
endWhile2:
; wain body (expr)
lis $3
.word 0

; epilogue: wain
add $30 ,$30 ,$4
add $30 ,$30 ,$4
add $30 ,$30 ,$4
add $30 ,$30 ,$4
jr $31
; procedure
FfastSort:
sub $29, $30, $4 ; update frame pointer for this procedure
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; if statement
; comparison
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, else3
; lvalue statement
; assignment through id
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, endif3
else3:
endif3:
; if statement
; comparison
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, else4
; lvalue statement
; assignment through id
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, endif4
else4:
endif4:
; if statement
; comparison
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $3, $5
sub $3, $11, $3
beq $3, $0, else5
; lvalue statement
; assignment through id
lis $3
.word 1
sw $3, 0($29)
beq $0, $0, endif5
else5:
endif5:
; if statement
; comparison
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
add $30, $30, $4
lw $5, -4($30)
slt $6, $3, $5
slt $7, $5, $3
add $3, $6, $7
sub $3, $11, $3
beq $3, $0, else6
; lvalue statement
; assignment through id
; calling procedure
; saving registers
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word Fpartition
jalr $5
; restoring registers
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, -4($29)
; lvalue statement
; assignment through id
; calling procedure
; saving registers
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
; restoring registers
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, 0($29)
; lvalue statement
; assignment through id
; calling procedure
; saving registers
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
; binary op
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word FfastSort
jalr $5
; restoring registers
add $30, $30, $4
lw $31, -4($30)
add $30, $30, $4
lw $29, -4($30)
sw $3, 0($29)
beq $0, $0, endif6
else6:
endif6:
lis $3
.word 0

; epilogue procedure: fastSort
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
jr $31
; procedure
Fpartition:
sub $29, $30, $4 ; update frame pointer for this procedure
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; adding var dcl
lis $3
.word 0
sw $3, -4($30)
sub $30, $30, $4
; lvalue statement
; assignment through id
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, 8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
sw $3, 0($29)
; lvalue statement
; assignment through id
; binary op
lw $3, 8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -4($29)
; lvalue statement
; assignment through id
; binary op
lw $3, 4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -8($29)
; while loop
loop7:
; comparison
lw $3, -16($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 0
add $30, $30, $4
lw $5, -4($30)
slt $6, $3, $5
slt $7, $5, $3
add $3, $6, $7
sub $3, $11, $3
beq $3, $0, endWhile7
; lvalue statement
; assignment through id
; binary op
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($29)
; while loop
loop8:
; comparison
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
sw $3, -4($30)
sub $30, $30, $4
lw $3, 0($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
beq $3, $0, endWhile8
; lvalue statement
; assignment through id
; binary op
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($29)
beq $0, $0, loop8
endWhile8:
; lvalue statement
; assignment through id
; binary op
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -8($29)
; while loop
loop9:
; comparison
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
sw $3, -4($30)
sub $30, $30, $4
lw $3, 0($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $3, $5
beq $3, $0, endWhile9
; lvalue statement
; assignment through id
; binary op
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3
sw $3, -8($29)
beq $0, $0, loop9
endWhile9:
; if statement
; comparison
lw $3, -4($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
add $30, $30, $4
lw $5, -4($30)
slt $3, $5, $3
sub $3, $11, $3
beq $3, $0, else10
; lvalue statement
; assignment through id
lw $3, -8($29)
sw $3, -20($29)
; lvalue statement
; assignment through id
lis $3
.word 241
sw $3, -16($29)
beq $0, $0, endif10
else10:
endif10:
; if statement
; comparison
lw $3, -16($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 241
add $30, $30, $4
lw $5, -4($30)
slt $6, $3, $5
slt $7, $5, $3
add $3, $6, $7
beq $3, $0, else11
; lvalue statement
; assignment through id
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
sw $3, -12($29)
; lvalue statement
; assignment through pointer deref
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
lw $3, 0($3) ; deref ptr
sw $3, -4($30)
sub $30, $30, $4
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
; lvalue statement
; assignment through pointer deref
lw $3, -12($29)
sw $3, -4($30)
sub $30, $30, $4
; binary op
; plus op: ptr + int
lw $3, 12($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -8($29)
mult $3, $4
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
add $30, $30, $4
lw $5, -4($30)
sw $5, 0($3)
beq $0, $0, endif11
else11:
endif11:
beq $0, $0, loop7
endWhile7:
lw $3, -20($29)

; epilogue procedure: partition
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
jr $31
