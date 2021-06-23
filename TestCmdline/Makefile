TGT := cltest
CC := g++
CCFLAGS := -std=c++17 -O3 -W -Wall -Wno-deprecated -DNODEBUG
INCLUDEPATH :=
LIBPATH :=
LIBS :=

################################################################
SRCS   = $(wildcard *.cpp) # wildcard関数を用いてファイル内の.cppを全て取得する(配列)
OBJS   = $(SRCS:.cpp=.o) # "<ファイル名>.o"の配列を作る
DEPS   = $(OBJS:.o=.d) # "<ファイル名>.d"の配列を作る


all: $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)

.cpp.o:
	$(CC) $(CCFLAGS) $(INCLUDEPATH) -MMD -MP -MF $(<:%.cpp=%.d) -c $< -o $(<:%.cpp=%.o)

.c.o:
	$(CC) $(CCFLAGS) $(OHTERFLAGS) -MMD -MP -MF $(<:%.cpp=%.d) -c $< -o $(<:%.cpp=%.o)

debug:
	make all CCFLAGS="-ggdb -W -Wall -pedantic"


.PHONY: clean
clean:
	$(RM) $(TGT) $(OBJS) $(DEPS)

-include $(DEPS)


