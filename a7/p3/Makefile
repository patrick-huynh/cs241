run:
	g++ -std=c++17 wlp4gen.cc -o wlp4gen
	wlp4scan < input.wlp4 | wlp4parse | wlp4type > input.wlp4ti
	wlp4c < input.wlp4 > expected.mips
	./wlp4gen < input.wlp4ti > output.asm
	cs241.binasm < output.asm > output.mips
	mips.twoints output.mips
	