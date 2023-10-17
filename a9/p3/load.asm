.import readWord
.import printHex

lis $10
.word readWord
lis $11
.word printHex

lis $4
.word 4
lis $22
.word 12

sw $31, -4($30)
sub $30, $30, $4

jalr $10 ; read mem address alpha
add $5, $3, $0 ; put mem address at $5
add $6, $3, $0 ; put mem address at $6


jalr $10 ; header1
jalr $10 ; header2
jalr $10 ; header3
add $13, $3, $0 ; endCode label

sub $13, $13, $22

loop:
beq $13, $0, exec
jalr $10 ; read from input
add $1, $3, $0 ; put word in param for printHex
sub $13, $13, $4 ; decrement
jalr $11 ; call printHex
sw $1, 0($5) ; save word to mem address
add $5, $5, $4 ; update mem
beq $0, $0, loop

exec:
; calling procedure
; save reg
sw $5, -4($30)
sub $30, $30, $4
sw $6, -4($30)
sub $30, $30, $4

jalr $6

; restore reg
lis $4
.word 4
lis $11
.word printHex

add $30, $30, $4
lw $6, -4($30)
add $30, $30, $4
lw $5, -4($30)

loop2:
beq $5, $6, end
lw $1, 0($6) ; load word for printHex
jalr $11 ; call printHex
add $6, $6, $4 ; update mem pointer
beq $0, $0, loop2 ; loop back

end:
add $30, $30, $4
lw $31, -4($30)
jr $31