lis $3
.word 0
beq $1, $3, 3
divu $2, $1
mfhi $4
bne $4, $3, 2
lis $3
.word 1
jr $31
