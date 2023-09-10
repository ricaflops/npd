#############################################
# npd
#############################################
# Build release version:     make
# Build debug version:       make debug=1
# Clean release build files: make clean
# Clean release build files: make debug=1 clean

#############################################
##### EXECUTABLE

EXEC := npd

#############################################
##### DIRECTORIES

SRCDIR     := ./src
BUILDDIR   := ./build
RELEASEDIR := release
DEBUGDIR   := debug

ifeq ($(debug),1)
  BUILDTYPE := Debug
  BUILDDIR := $(BUILDDIR)/$(DEBUGDIR)
else
  BUILDTYPE := Release
  BUILDDIR := $(BUILDDIR)/$(RELEASEDIR)
endif

#############################################
##### TOOLS

CXX := g++
RM_CMD := rm -rf
MKDIR_CMD := mkdir -p

#############################################
##### COMPILE SETTINGS

CXXFLAGS := -m64 -std=c++11 -pipe -Wall
LDFLAGS := 

ifeq ($(debug),1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O2
  LDFLAGS += -s
endif

#############################################
##### FILES

SRCS := $(wildcard $(SRCDIR)/*.cpp)
 
OBJS := $(subst .cpp,.o, $(subst $(SRCDIR),$(BUILDDIR),$(SRCS)))

DEPS := $(OBJS:%.o=%.d)

#############################################
##### TARGETS

.PHONY: all clean

all: $(EXEC)

clean:
	@$(RM_CMD) $(EXEC)
	@$(RM_CMD) $(BUILDDIR)
	@echo $(BUILDTYPE) build cleaned

#############################################
##### RULES

# Link
$(EXEC): $(OBJS)
	@echo Linking $(BUILDTYPE): $@
	@$(CXX) $(LDFLAGS) -o $@ $^

# Compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	@echo Compiling $(BUILDTYPE): $<
	@$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@
	
# Dependencies
-include $(DEPS)

# Create directories
$(BUILDDIR):
	@echo Creating $(BUILDTYPE) build directories
	@$(MKDIR_CMD) $(BUILDDIR)
