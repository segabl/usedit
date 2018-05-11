CFLAGS = -Wall -O3 -std=c++1y
LDFLAGS = -llua -lsfml-graphics -lsfml-window -lsfml-system -laudiere -lcomdlg32

SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJS = $(SRC:src/%.cpp=obj/%.o)

build_folders := $(shell mkdir -p obj/gui > /dev/null 2>&1)
ifeq ($(OS),Windows_NT)
	NAME = USEdit.exe
else
	NAME = USEdit
endif

CLEANCMD = rm $(NAME) obj -r > /dev/null 2>&1

#default build
all: $(OBJS)
	g++ $(OBJS) -o $(NAME) $(LDFLAGS)

obj/%.o: src/%.cpp
	g++ -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	@$(CLEANCMD)
