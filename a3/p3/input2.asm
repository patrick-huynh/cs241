first: ; 0x00
beq $0, $0, first ; jump -1
second:
beq $0, $0, first ; jump -2
.word second ; 0x04