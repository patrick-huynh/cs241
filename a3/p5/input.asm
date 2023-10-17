bne $2, $4, 1
mult $4, $21
multu $4, $21
div $4, $21
divu $4, $21
mfhi $31
mflo $4
lis $23
.word 2
jr $31
jalr $0
lw $3, 4($31)
lw $3, -4($31)
lw $3, 0xffff ($31)
sw $2, 4($31)
sw $31, -4($31)
sw $21, 0xffff($31)