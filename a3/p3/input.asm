a: b:
beq $4, $3, j
.word a
.word b
c: ; comment
.word c
d: .word d
.word e
e: ; e is for end
f: g: h: beq $0, $0, f ; comment
.word a
j: i: .word b ; comment
.word i
.word -1