
sw $31, -4($30) ; add pc to stack
lis $31
.word 4
sub $30, $30, $31

lis $29
.word print
jalr $29

lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31
;; End main --------------------


print:
; print to stdout $1 in base 10
; Parameters
; $1 - two's complement integer
; Preserve
; $1, $10, $11, $12 

; Add registers to stack
sw $10, -4($30)
sw $11, -8($30)
sw $12, -12($30)
sw $13, -16($30)
sw $14, -20($30)
sw $15, -24($30)
sw $16, -28($30)

sw $20, -32($30)
sw $21, -36($30)
sw $22, -40($30)
sw $23, -44($30)
sw $24, -48($30)
sw $25, -52($30)

lis $10
.word 52
sub $30, $30, $10

; Constants
lis $10 ; ascii '0'
.word 48
lis $11 ; ascii '-'
.word 45
lis $12 ; ascii linefeed
.word 0x0A
lis $13 ; stdout
.word 0xffff000c
lis $14
.word -1
lis $15
.word 10
lis $16
.word 1

; Variables
lis $20 ; $20 = 1 if $1 < 0
.word 0
add $21, $0, $1 ; copy $1 to $21
add $22, $0, $0 ; quotient tracker
add $23, $0, $0 ; remainder tracker
add $24, $0, $0 ; num digits
add $25, $0, $0 ; digit tracker

; Body
; Prints '-' if $1 < 0
slt $20, $1, $0 ; if $1 < 0, then $20 = 1
beq $20, $0, getdigit ; branch if $20 is not negative
sw $11, 0($13) ; prints '-'

getdigit:
numdigit: ; determines num of digits
div $21, $15 ; $21 / 10
add $24, $24, $16 ; $24++
mflo $21 ; copy quotient to $22
bne $21, $0, numdigit ; loop when quotient is not equal to 0


; sw $24, 0($13) ; test
; $24 now holds the num of digits

printdigit:
add $21, $0, $1 ; reset $21
add $25, $0, $0 ; reset digit tracker to 0
loop:
div $21, $15 ; $21 / 10
add $25, $25, $16 ; $25++
mflo $21 ; copy quotient to $21
bne $25, $24, loop
mfhi $23 ; copy remainder, or the output digit
beq $20, $0, outputdigit ; branch if not negative
div $23, $14 ; $24 / -1
mflo $23 ; makes it positive

outputdigit:
add $23, $23, $10 ; converts to ascii
sw $23, 0($13) ; prints the first digit
sub $24, $24, $16; $24--
bne $24, $0, printdigit
 
end: ; end of function print
sw $12, 0($13) ; prints linefeed

; Restore registers from stack
lis $10
.word 52
add $30, $30, $10
lw $10, -4($30)
lw $11, -8($30)
lw $12, -12($30)
lw $13, -16($30)
lw $14, -20($30)
lw $15, -24($30)
lw $16, -28($30)

lw $20, -32($30)
lw $21, -36($30)
lw $22, -40($30)
lw $23, -44($30)
lw $24, -48($30)
lw $25, -52($30)

jr $31


