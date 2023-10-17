beq $0, $0, skip
.word 0
skip: lis $3
.word 241
lis $4
.word 0x10004
sw $3, 0($4)
jr $31