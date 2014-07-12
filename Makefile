CC	:= g++
LD	:= g++

OUT			:= bin
OBJ_DIR		:= $(OUT)/objs
INC_DIRS	:= $(CUDA_PATH)/include
INC_DIRS	+= $(AF_PATH)/include
LIB_DIRS	:= $(AF_PATH)/lib64
LIBRARIES	:= glfw
LIBRARIES	+= GL
LIBRARIES	+= GLEW
LIBRARIES	+= OpenCL
LIBRARIES	+= afcl

INC_FLAGS	:= $(patsubst %,-I%, $(INC_DIRS))
CPP_FLAGS	:= $(patsubst %,-L%, $(LIB_DIRS))
LIB_FLAGS	:= $(patsubst %,-l%, $(LIBRARIES))

SRC := $(wildcard Fractal/*.cpp)
SRC += $(wildcard Fractal/stb_image/*.c)
OBJ := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

.PHONY: all clean
	
all: main-build

clgl_demo: $(OBJ)
	$(LD) -o $(OUT)/$@ $^ $(CPP_FLAGS) $(LIB_FLAGS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(INC_FLAGS) -c $< -o $@

pre-build:
	@mkdir -p $(OUT) $(OBJ_DIR)/Fractal $(OBJ_DIR)/Fractal/stb_img

main-build: pre-build
	@$(MAKE) --no-print-directory clgl_demo

clean:
	@rm bin -rf
