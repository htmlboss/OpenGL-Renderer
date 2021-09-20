
output_dir := ./build
create_output_dir := mkdir -p $(output_dir)

glm_dir := ./MP-APS/3rdParty/glm/
glad_dir := ./MP-APS/3rdParty/glad/include/
glad_src := ./MP-APS/3rdParty/glad/src/glad.c
stb_dir := ./MP-APS/3rdParty/stb/
pugixml_dir := ./MP-APS/3rdParty/pugixml/include/
nuklear_dir := ./MP-APS/3rdParty/nuklear/include/

libs := -lstdc++fs -lGL -lglfw -lpthread -lassimp

all:
	make clean
	$(create_output_dir)
	g++ -std=c++17 -Wall -Wextra -Wno-unused-variable -march=native -isystem -O3 -o $(output_dir)/MP-APS -I$(glm_dir) -I$(glad_dir) -I$(stb_dir) -I$(pugixml_dir) -I$(nuklear_dir) ./MP-APS/*.cpp ./MP-APS/Core/*.cpp ./MP-APS/Demos/*.cpp ./MP-APS/Graphics/*.cpp $(glad_src) $(libs)
	cp -r ./MP-APS/Data/ $(output_dir)

clean:
	rm -rf $(output_dir)
