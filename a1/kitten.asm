lis $8
.word 0xffff0004 ; std in special address
lis $9
.word 0xffff000c ; std out special address
lis $4
.word 0xffffffff ; $4 = -1
add $3, $3, $0 ; $3 = 0
lis $7
.word 1 ; $7 = 1
input:
lw $10, 0($8) ; load from std in into $10
beq $10, $4, end; if $10 == -1 end the loop
add $3, $3, $7 ; $3 += 1 increment the size
sw $10, 0($9) ; send $10 to std output
bne $10, $4, input ; if $10 != -1, get more input
end:
jr $31
