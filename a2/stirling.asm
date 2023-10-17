; $1 = n 
; $2 = k
stirling:
; Preserving
sw $1, -4($30)
sw $2, -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $10, -20($30)
sw $11, -24($30)
sw $31, -28($30)

lis $4
.word 28
sub $30, $30, $4

; Constants
lis $10
.word 1
lis $11
.word stirling


; Body
beq $1, $0, base
beq $2, $0, base

; recursive case
sub $1, $1, $10 ; $1 = n-1
jalr $11 ; call stirling f(n-1,k)
multu $1, $3 ; (n-1) * f(n-1,k)
mflo $5

sub $2, $2, $10 ; $2 = k-1
jalr $11 ; call stirling f(n-1, k-1)
add $3, $5, $3
beq $0, $0, end

; base case
base: ; base case when n == 0 or k == 0
; by default, assume both nums are 0
add $3, $0, $10 ; $3 = 1

; if one of the nums is non-zero
bne $1, $0, onezero
bne $2, $0, onezero
beq $0, $0, end ; when both nums are 0

onezero:
add $3, $0, $0 ; $3 = 0

end:
; Restoring
lis $4
.word 28
add $30, $30, $4
lw $1, -4($30)
lw $2, -8($30)
lw $4, -12($30)
lw $5, -16($30)
lw $10, -20($30)
lw $11, -24($30)
lw $31, -28($30)
jr $31
