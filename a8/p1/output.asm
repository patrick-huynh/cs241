; prologue
; .import print
lis $4 ; $4 will always hold 4
.word 4
; lis $10 ; $10 will always hold address for print
; .word print
lis $11 ; $1 will always hold 1
.word 1
sub $29, $30, $4 ; setup frame pointer
sw $1, 0($29) ; push first param
sub $30 , $30 , $4 ; update stack ptr
sw $2, -4($29) ; push second param
sub $30 , $30 , $4 ; update stack ptr
; calling procedure
; saving registers
sw $29, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lis $3
.word 2
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word Ff
jalr $5
; restoring registers
add $30,$30,$4
lw $31, -4($30)
add $30,$30,$4
lw $29, -4($30)

; epilogue: wain 
add $30 ,$30 ,$4
add $30 ,$30 ,$4
jr $31
; procedure
Ff:
sub $29, $30, $4 ; update frame pointer for this procedure
lw $3, 4($29)

; epilogue procedure: f
add $30, $30, $4
jr $31
