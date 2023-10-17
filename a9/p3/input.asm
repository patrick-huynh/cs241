lis $3
.word 0xffff000c
lis $2
.word 0x21
lis $1
.word 0x0a
sw $2, 0($3)
sw $1, 0($3)
jr $31