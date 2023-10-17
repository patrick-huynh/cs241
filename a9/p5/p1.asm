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

jalr $10 ; header
jalr $10 ; header
add $13, $3, $0 ; endCode label
jalr $10 ; header

sub $13, $13, $4
sub $13, $13, $4

loop:
jalr $10
add $1, $3, $0
sub $13, $13, $4
beq $13, $0, end
jalr $11
beq $0, $0, loop

end:
add $30, $30, $4
lw $31, -4($30)
jr $31
