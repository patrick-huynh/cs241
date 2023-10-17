; start prologue
; .import print
lis $4 ; $4 will always hold 4
.word 4
; lis $10 ; $10 will always hold address for print
; .word print
lis $11 ; $1 will always hold 1
.word 1
sub $29, $30, $4 ; setup frame pointer
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
; while loop
loop1:
; comparison
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 3
add $30, $30, $4
lw $5, -4($30)
slt $3, $3, $5
sub $3, $11, $3
beq $3, $0, endWhile1
; lvalue statement
; binary op
lw $3, -8($29)
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 1
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -8($29)
beq $0, $0, loop1
endWhile1:
; wain body (expr)
lw $3, -8($29)

; start epilogue
add $30 ,$30 ,$4
add $30 ,$30 ,$4
add $30 ,$30 ,$4
jr $31
