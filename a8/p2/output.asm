; start prologue
.import print
lis $4 ; $4 will always hold 4
.word 4
lis $10 ; $10 will always hold address for print
.word print
lis $11 ; $1 will always hold 1
.word 1
sub $29, $30, $4 ; setup frame pointer
; end prologue

sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
; wain body (expr)
add $3, $0, $11 ; NULL is always 1
lw $3, 0($3) ; deref ptr

; epilogue: wain
add $30 ,$30 ,$4
add $30 ,$30 ,$4
jr $31
