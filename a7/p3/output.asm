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
; wain body (expr)
; binary op
; binary op
lis $3
.word 240
sw $3, -4($30)
sub $30, $30, $4
; binary op
lis $3
.word 220
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 220
add $30, $30, $4
lw $5, -4($30)
div $5, $3
mflo $3
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
sw $3, -4($30)
sub $30, $30, $4
; binary op
lis $3
.word 2410
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 10
add $30, $30, $4
lw $5, -4($30)
div $5, $3
mflo $3
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3

; start epilogue
add $30 ,$30 ,$4
add $30 ,$30 ,$4
jr $31
