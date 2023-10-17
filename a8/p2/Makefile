run:
	g++ -std=c++17 wlp4gen.cc -o wlp4gen
	wlp4scan < input.wlp4 | wlp4parse | wlp4type > input.wlp4ti
	wlp4c < input.wlp4 > expected.mips
	./wlp4gen < input.wlp4ti > output.asm
	cs241.linkasm < output.asm > output.merl
	cs241.linker output.merl print.merl > linked.merl
	# mips.twoints linked.merl
	mips.array linked.merl
	