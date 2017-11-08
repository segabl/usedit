CFLAGS = -Wall -O3 -std=c++1y
LDFLAGS = -llua -lsfml-graphics -lsfml-window -lsfml-system -laudiere -lcomdlg32

SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJS = $(SRC:src/%.cpp=obj/%.o)

build_folders := $(shell mkdir obj\gui >nul 2>&1)
NAME = USEdit.exe
CLEANCMD = del obj\*.o /s > nul 2>&1

#default build
all: $(NAME)

$(NAME): $(OBJS)
	g++ $(OBJS) -o $@ $(LDFLAGS)

#special windows build to include exe information
windows: $(OBJS)
	windres src/resource.rc -O coff -o obj/resource.res
	g++ $(OBJS) obj/resource.res -o $(NAME) $(LDFLAGS)

obj/%.o: src/%.cpp
	g++ -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	@$(CLEANCMD)
