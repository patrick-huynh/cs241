run:
	g++ -std=c++17 wlp4type.cc -o wlp4type
	wlp4scan < program.wlp4 | wlp4parse > program.wlp4i
	wlp4scan < program.wlp4 | wlp4parse | wlp4type > expected.wlp4ti
	./wlp4type < program.wlp4i > output.wlp4ti