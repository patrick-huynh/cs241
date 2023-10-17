; Valid program, don't produce an ERROR!

; .word
.word 241

; add and beq
add $1, $2, $3
beq $3, $2,  1

; labels
label: .word label