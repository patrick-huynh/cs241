.export a

lis $3
.word a
sw $0, 0($3)
sw $3, 0($0)
jr $31
a: .word 0xa