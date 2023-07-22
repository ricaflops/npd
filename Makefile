#############################################
# npd Makefile
#############################################
# 'make' creates debug version
# 'make release=1' creates release version
# 'make clean' erase build files

#############################################
## Executable file
#############################################
EXEC := npd

#############################################
## Directories
#############################################
SRC_DIR := ./src
BUILD_DIR := ./build

#############################################
## Shell tools
#############################################
RM_CMD := rm -rf
MKDIR_CMD := mkdir -p

#############################################
## Compiler settings
#############################################
CC := gcc
CXX := g++
CXXFLAGS := -m64 -std=c++17 -pipe -Wall
LDFLAGS := 

ifeq ($(release),1)
	CXXFLAGS += -O3 -s
	BUILD_TYPE := Release
else
	CXXFLAGS += -g
	BUILD_TYPE := Debug
endif

#############################################
## Files
#############################################

# List of all .cpp files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
 
# Object files. One for each .cpp file
OBJS := $(subst .cpp,.o, $(subst $(SRC_DIR),$(BUILD_DIR),$(SRCS)))

# Dependency files. One for each .o file
DEPS := $(OBJS:%.o=%.d)

#############################################
## Rules
#############################################

# Linker: *.o --> executable
$(EXEC): $(OBJS)
	@echo Linking: $@
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo Done: $(BUILD_TYPE) build

# Compiling rule: .cpp --> .o .d
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo Compiling: $<
	@$(CXX) $(LDFLAGS) $(CXX_FLAGS) -MMD -c $< -o $@
	
# Include dependency rules (.d files)
-include $(DEPS)

# Create build directory
$(BUILD_DIR):
	@$(MKDIR_CMD) $(BUILD_DIR)

#############################################
## Targets
#############################################

.PHONY: all clean

# Create executable
all:
	$(EXEC)

# Cleanup
clean:
	@$(RM_CMD) $(EXEC)
	@$(RM_CMD) $(BUILD_DIR)
	@echo Cleaned.

