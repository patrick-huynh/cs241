
; $1 = starting address
; $2 = size

; Constants
lis $10
.word 2
lis $11
.word 1
lis $12
.word 4
lis $13
.word 1

sub $5, $2, $13 ; counter for the array

add $21, $0, $1 ; copy the starting address over
add $22, $0, $2 ; copy the size over

sw $31, -4($30) ; add pc to stack
lis $31
.word 4
sub $30, $30, $31

check:
lw $1, 0($21)
lw $2, 4($21)
lis $29
.word compare
jalr $29 ; calling compare procedure
add $21, $21, $12 ; update address 
sub $5, $5, $13 ; update counter
beq $5, $0, end ; when end off array is reached
bne $3, $0, check 

end: 
lis $31 ; restore pc from stack
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31
