run:
	cs241.linkasm < input.asm > input.merl
	cs241.linkasm < p1.asm > p1.merl
	cs241.linker p1.merl starter.merl > linked.merl
	mips.stdin linked.merl < input.merl