; Contstants
lis $10 ; address of stdin
.word 0xffff0004
lis $11 ; address of stdout
.word 0xffff000c
lis $12 ; eof
.word -1
lis $13 ; $13 = 1
.word 1
add $3, $0, $0 ; $3 = 0 = size tracker

;; start

sw $31, -4($30) ; saves pc to stack
lis $31
.word 4
sub $30, $30, $31

lis $5
.word read
jalr $5 ; call read

lis $31 ; restore pc from stack
.word 4
add $30, $30, $31
lw $31, -4($30)

jr $31
;; end


; read: reads input and prints them in reverse
read:
; preserve $2
sw $2, -4($30) ; save $2 to stack
sw $31, -8($30) ; save pc to stack
lis $2
.word 8
sub $30, $30, $2

; Body
lw $2, 0($10) ; reads input
beq $2, $12, print ; if not eof -> recurse on the next input

lis $6
.word read
jalr $6 ; recurse

;add $3, $3, $13; update size to $3
; sw $2, 0($11) ; sends to stdout

print:
beq $2, $12, end ; if eof skip print
add $3, $3, $13 ; update size to $3
sw $2, 0($11) ; print

end:
lis $2 ; restores $2
.word 8
add $30, $30, $2
lw $2, -4($30)
lw $31, -8($30)
jr $31
; end function

