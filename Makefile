# directory definitions
output_dir := ./build

APPNAME := $(output_dir)/MP-APS

# collect all source files
SRC += $(wildcard ./MP-APS/*.cpp)
SRC += $(wildcard ./MP-APS/Core/*.cpp)
SRC += $(wildcard ./MP-APS/Demos/*.cpp)
SRC += $(wildcard ./MP-APS/Graphics/*.cpp)
SRC += ./MP-APS/3rdParty/fmt/src/format.cc
SRC += ./MP-APS/3rdParty/glad/src/glad.c

# Use implicit rules to build object files from source files.
# We have *.cpp *.cc *.c source files.
OBJ := $(SRC:.cpp=.o)
OBJ := $(OBJ:.cc=.o)
OBJ := $(OBJ:.c=.o)

CPPFLAGS += -I./MP-APS/3rdParty/glm
CPPFLAGS += -I./MP-APS/3rdParty/glad/include
CPPFLAGS += -I./MP-APS/3rdParty/stb
CPPFLAGS += -I./MP-APS/3rdParty/fmt/include
CPPFLAGS += -I./MP-APS/3rdParty/pugixml/include
CPPFLAGS += -I./MP-APS/3rdParty/nuklear/include

CXXFLAGS += -std=c++17 -Wall -Wextra -Wno-unused-variable -march=native -isystem -O3

LDFLAGS +=

LDLIBS += -lstdc++fs -lGL -lglfw -lpthread -lassimp -ltbb

# virtual targets
.PHONY : all clean

all: $(APPNAME) copydata

$(output_dir):
	mkdir -p $(output_dir)

copydata: $(output_dir)
# only update (this is faster, if already copied once)
	cp -r -u ./MP-APS/Data $(output_dir)

$(APPNAME): $(OBJ) $(output_dir)
	$(CXX) $(OBJ) -o $(APPNAME) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJ)
	$(RM) -r $(output_dir)
