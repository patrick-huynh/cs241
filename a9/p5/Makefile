run:
	# cs241.binasm <<< '.word 0x10000' > address.bin
	cs241.linkasm < input.asm > input.merl
	cat address.bin input.merl > input.in
	cs241.linkasm < load.asm > load.merl
	cs241.linker load.merl starter.merl > linked.merl
	mips.stdin linked.merl < input.in