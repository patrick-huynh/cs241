.import readWord
.import printHex

lis $10
.word readWord
lis $11
.word printHex

lis $4
.word 4

sw $31, -4($30)
sub $30, $30, $4

jalr $10 ; read mem address alpha
add $5, $3, $0 ; put mem address at $5
add $6, $3, $0 ; put mem address at $6


jalr $10 ; header1
jalr $10 ; header2
add $13, $3, $0 ; endCode label
jalr $10 ; header3

sub $13, $13, $4
sub $13, $13, $4

loop:
jalr $10 ; read from input
add $1, $3, $0 ; put word in param for printHex
sub $13, $13, $4 ; decrement
beq $13, $0, loop2
jalr $11 ; call printHex
sw $1, -4($5) ; save word to mem address
sub $5, $5, $4 ; update mem
beq $0, $0, loop

loop2:
beq $5, $6, end
lw $1, -4($6) ; load word for printHex
jalr $11 ; call printHex
sub $6, $6, $4 ; update mem pointer
beq $0, $0, loop2

end:
add $30, $30, $4
lw $31, -4($30)
jr $31
