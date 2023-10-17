; Constants
lis $20
.word -1
lis $21
.word 1
lis $22
.word 4
lis $29
.word findHeight
lis $28
.word max

add $9, $0, $1

;; findheight function finds the height of a tree
;; Parameter: $9 - The address of the node of a tree
;; Preserve $5 ,$6, $31
findHeight:
sw $5, -4($30)
sw $6, -8($30)
sw $31, -12($30)
sw $7, -16($30)
sw $8, -20($30)
sw $9, -24($30)
sw $4, -28($30)
lis $5
.word 28
sub $30, $30, $5


; loading in indices of the nodes children
lw $5, 4($9) ; load index of left child
lw $6, 8($9) ; load index of right child

lw $4, 0($9) ; value of node

; base case - checks if both children exist
bne $5, $20, recurse
bne $6, $20, recurse
add $3, $0, $21 ; return value 1 for the current node
beq $0, $0, end

; recursive case
recurse:
add $7, $0, $0
add $8, $0, $0

beq $5, $20, rightChild ; no left child exists
mult $5, $22 ; loading value of le
mflo $7
add $9, $1, $7 ; address of leftChild
jalr $29 ; findHeight(leftChild)
add $7, $21, $3 ; $7 = 1 + findHeight(leftChild)

rightChild:
beq $6, $20, endRecurse ; no right child exists
mult $6, $22
mflo $8
add $9, $1, $8 ; address of rightChild
jalr $29 ; findHeight(rightChild)
add $8, $21, $3 ; $8 = 1 + findHeight(rightChild)


endRecurse:
add $10, $0, $7
add $11, $0, $8
jalr $28
add $3, $0, $12 ; $3 = max($5, $6)

end:
lis $5
.word 28
add $30, $30, $5
lw $5, -4($30)
lw $6, -8($30)
lw $31, -12($30)
lw $7, -16($30)
lw $8, -20($30)
lw $9, -24($30)
lw $4, -28($30)
jr $31


;; max function returns the large number in $12
;; max($10,$11)
max:
slt $12, $10, $11
bne $12, $0, second
add $12, $0, $10
beq $0, $0, endmax
second:
add $12, $0, $11
endmax:
jr $31
