lis $1 ; stdin address
.word 0xffff0004
lis $2
.word 0xffff000c ; stdout address

; Constants
lis $20 ; $20 = 10 = "line feed"
.word 10
lis $21 ; $21 = 45 = '-' ascii for dash
.word 45
lis $4
.word 48 ; ascii value for '0' 
lis $6
.word -1
lis $7 ; $7 = 2
.word 2
lis $8 ; $8 = 256
.word 256
lis $9 ; $9 = 1
.word 1

add $5, $0, $0 ; $5 = 1, if input is neg, else 0 

add $15, $0, $0 ; $15 = 0

; Body
; Reads input and stores integer value in $15
read:
lw $10, 0($1) ; reading input
beq $10, $6, doneread ; if eof, go to doneread
beq $10, $20, doneread ; if line feed go to doneread
bne $21, $10, skipneg ; checks if the first input is '-'
add $5, $9, $0 ; $5 = 1, if input is neg
beq $0, $0, read ; go back to read

skipneg:
sub $10, $10, $4 ; convert ascii to int
multu $15, $20 ; multiplies $15 by 10 for every iteration
mflo $15
add $15, $15, $10 ; add the new int to total val
beq $0, $0, read ; go back to read

doneread:
; Now uses the dividing method to determine binay output
; Checks if val is negative if $5 = 1
add $29, $0, $0 ; $29 = quotient tracker
add $26, $0, $15 ; $26 = $15
lis $25 ; checks if padding of zeroes is necessary
.word 8
bne $5, $9, pad0
sub $15, $8, $15 ; since val is negative
add $26, $0, $15 ; reset $26

pad0:

sw $15, 0($2); END
jr $31

divu $26, $7 ; div 2 
mflo $26
sub $25, $25, $9 ; decrement pad counter
bne $26, $0, pad0

printpad:
beq $25, $0, endpad
sw $0, 0($2) ;; EDIT
sub $25, $25, $9 ; decrement pad counter
beq $0, $0, printpad

endpad:
add $26, $0, $15 ; reset

getbits:
divu $26, $7 ; divide by 2
mfhi $28 ; $28 = remainder
mflo $27 ; $27 = quotient
add $26, $0, $27
bne $27, $29, getbits
add $24, $28, $0 ; converts to ascii
sw $24, 0($2) ; EDIT
add $29, $0, $27
multu $27, $7 ; quotient x 2
mflo $29
add $29, $29, $28 ; updates new quotient threshold
add $26, $0, $15 ; reset $26
bne $29, $15, getbits

linefeed:
;sw $20, 0($2) ; print line feed
jr $31
