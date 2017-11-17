
# all: dirs
# 	@echo choose target 'sdl_test' or 'sfml_test' which doesn\'t work yet
	
build := build

cpp = \
	UI/Gui.cpp \
	UI/Control.cpp \
	UI/Widget.cpp \
	\
	UI/common.cpp \
	UI/managers/ControlManager.cpp \
	UI/managers/Fonts.cpp \
	UI/managers/Images.cpp \
	UI/managers/Sounds.cpp \
	UI/managers/Effects.cpp \
	UI/Color.cpp \
	UI/ColorString.cpp \
	UI/Cursor.cpp \
	UI/Image.cpp \
	UI/BasicImage.cpp \
	UI/Sound.cpp \
	UI/TiledImage.cpp \
	UI/TiledFont.cpp \
	\
	UI/Font.cpp \
	UI/backend/Speaker.cpp \
	UI/backend/Screen.cpp \
	
	
	

exe := 
link := 
defs := 

.PHONY: dirs, sfml_test, sdl_test

build := build

flags := -g -Wfatal-errors
inc := -Ilib

CXX := 
CFLAGS := 

obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(cpp)))

controls_path := UI/controls

# all controls which contains only controls with .cpp code
control_names := Button 	    \
				RadioButton 	\
				ComboBox 		\
				CheckBox		\
				Label           \
				ScrollBar       \
				TextBox         \
				ListBox         \
				TrackBar        \
				Container       \
				Dialog			\
				Terminal		\
				Canvas			\
				WidgetMover		\
				WidgetResizer	\
				ProgressBar		\
				TabContainer    \

############ SFML ##############
# sfml_test_cpp := $(cpp) SFML_test.cpp \
# 		UI/common/SFML/Drawing.cpp \
# 		$(addsuffix .cpp, $(addprefix $(controls_path)/SFML/, $(control_names)))
		
# sfml_test_obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(sfml_test_cpp)))
# sfml_link := -lsfml-window -lsfml-graphics -lsfml-system -lGL
# sfml_defs := -DUSE_SFML
# sfml_test_exe := sfml_test

# sfml_test_build: defs := -DUSE_SFML
# sfml_test_build: $(sfml_test_obj)
# 	g++ $^ -o $@ $(sfml_link)
	
# sfml_test: dirs sfml_test_build


############ SDL ###############
sdl_test_cpp := $(cpp) \
		$(addsuffix .cpp, $(addprefix $(controls_path)/, $(control_names))) \
		UI/backend/SDL/SDLScreen.cpp \
		UI/backend/SDL/SDL_Surface_Image.cpp \
		UI/backend/SDL/SDL_TTF_Font.cpp \
		UI/backend/SDL/SDLSound.cpp \
		UI/backend/SDL/SDLSpeaker.cpp \
		UI/backend/SDL/SDLBackend.cpp \
		UI/backend/SDL/SDLSystem.cpp \
		UI/backend/drivers/SoundLibSnd.cpp \
		
		
sdl_test_obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(sdl_test_cpp)))
sdl_link := -lSDL2 -lSDL2_ttf -lSDL2_image -lGL -lGLEW -lpng -lsndfile
sdl_test_exe := sdl_test

sdl_test_build: defs :=
sdl_test_build: 
sdl_test_build: $(sdl_test_obj) build/SDL_test.o
	$(CXX)g++ $^ -o sdl_test $(sdl_link) $(CFLAGS)

sdl_test: dirs sdl_test_build

libout := libgui.a

sdl_lib_make: defs :=
sdl_lib_make: $(sdl_test_obj)
	$(CXX)ar r $(libout) $^

sdl_lib: dirs sdl_lib_make

###########################


	
clean:
	@echo project cleaned
	@rm -f libgui.a
	@rm -f sdl_test
	@rm -f sfml_test
	@rm -rf build
	
dirs:
	@mkdir -p $(build)
	@mkdir -p $(build)/UI
	@mkdir -p $(build)/UI/backend
	@mkdir -p $(build)/UI/backend/SDL
	@mkdir -p $(build)/UI/backend/drivers
	@mkdir -p $(build)/UI/managers
	@mkdir -p $(build)/UI/common
	@mkdir -p $(build)/UI/common/SDL
	@mkdir -p $(build)/UI/common/SFML
	@mkdir -p $(build)/UI/controls
	@mkdir -p $(build)/UI/controls/SDL
	@mkdir -p $(build)/UI/controls/SFML
	
$(build)/%.o: %.cpp
	$(CXX)g++ -c $< -o $@ -std=c++11 $(flags) $(defs) $(inc) $(CFLAGS)


