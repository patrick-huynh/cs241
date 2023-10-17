first:
.import readWord
.import printHex

lis $10
.word readWord
lis $11
.word printHex

lis $4
.word 4
lis $21
.word 1
lis $22
.word 12
lis $23
.word 0x10000
lis $24
.word 0x100000

; saving $31 for procedure calls
sw $31, -4($30)
sub $30, $30, $4


jalr $10 ; read mem address alpha
add $5, $3, $0 ; put mem address at $5
add $6, $3, $0 ; put mem address at $6


jalr $10 ; header1
jalr $10 ; header2
add $14, $3, $0 ; endModule label
jalr $10 ; header3
add $13, $3, $0 ; endCode label


sub $13, $13, $22
sub $14, $14, $4

loop:
beq $13, $0, reloc
jalr $10 ; read from input
add $1, $3, $0 ; put word in param for printHex
sub $13, $13, $4 ; decrement
sub $14, $14, $4 ; decrement
jalr $11 ; call printHex
sw $1, 0($5) ; save word to mem address
add $5, $5, $4 ; update mem
beq $0, $0, loop

reloc:
beq $14, $0, exec
jalr $10 ; read from input
bne $3, $21, else;
jalr $10 ; read from input - address to be relocated in $3
add $7, $6, $3 ; alpha + rel
sub $7, $7, $22 ; alpha + rel - 12
lw $8, 0($7) ; getting address and relocating
add $8, $8, $6 ; + alpha
sub $8, $8, $22 ; - 12
sw $8, 0($7) ; save back to mem address
else:
sub $14, $14, $4 ; first decrement
sub $14, $14, $4 ; second decrement
beq $0, $0, reloc

exec:
; calling procedure
; save reg
sw $5, -4($30)
sub $30, $30, $4
sw $6, -4($30)
sub $30, $30, $4

; storing $30 to be after the program
lis $4
.word first
add $4, $4, $24 ; first + 0x10 000
add $4, $4, $24 ; first + 0x10 000 + 0x10 000 
sw $30, 4($4)

add $30, $4, $0

jalr $6

; restore reg
lis $4
.word 4
lis $11
.word printHex
lis $22
.word 12
lis $23
.word 0x10000
lis $24
.word 0x100000

; setting $30 back after the program
lis $30
.word first
add $30, $30, $24
add $30, $30, $24
lw $30, 4($30)

add $30, $30, $4
lw $6, -4($30)
add $30, $30, $4
lw $5, -4($30)


loop2:
beq $5, $6, end
lw $1, 0($6) ; load word for printHex
jalr $11 ; call printHex
add $6, $6, $4 ; update mem pointer
beq $0, $0, loop2 ; loop back

end:
add $30, $30, $4
lw $31, -4($30)
jr $31