# =============================================================================
# Makefile — custom_dampers_lib STAR-CCM+ User Library
#
# Usage:
#   make                  → build libcustom_dampers.so
#   make clean            → remove build files
#   make info             → print and verify paths
# =============================================================================

# -----------------------------------------------------------------------------
# STAR-CCM+ paths — confirmed from your cluster ls output
# -----------------------------------------------------------------------------
STARCCM_INSTALL = /opt/Siemens/20.06.010/STAR-CCM+20.06.010
STARCCM_LIB_DIR = $(STARCCM_INSTALL)/star/lib/linux-x86_64-2.28/clang17.0/lib
STARCCM_SYS_DIR = $(STARCCM_INSTALL)/star/lib/linux-x86_64-2.28/system/clang17.0-64

# -----------------------------------------------------------------------------
# Compiler
# Use clang++ to match STAR-CCM+'s own toolchain (avoids GLIBCXX mismatch)
# If clang++ is not on PATH run: which clang++ or module load clang
# -----------------------------------------------------------------------------
CXX      = clang++
CXXFLAGS = -shared \
            -fPIC \
            -DDOUBLE_PRECISION \
            -std=c++17 \
            -O2 \
            -Wall

# Include directory (where uclib.h lives)
INCLUDES = -I./include

# Linker flags
# -Wl,-rpath embeds the runtime path so STAR-CCM+ finds libstdc++ on load
LDFLAGS  = -L$(STARCCM_LIB_DIR) \
            -L$(STARCCM_SYS_DIR) \
            -Wl,-rpath,$(STARCCM_SYS_DIR) \
            -lUserFunctions \
            -lm

# -----------------------------------------------------------------------------
# Source and output
# -----------------------------------------------------------------------------
SRC    = src/custom_dampers.cpp
TARGET = libcustom_dampers.so

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(SRC) include/uclib.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LDFLAGS)
	@echo ""
	@echo "  Built: $(TARGET)"
	@echo "  Load in STAR-CCM+: Tools → User Code → Add User Library"
	@echo ""

clean:
	rm -f $(TARGET)
	@echo "Cleaned."

# Verify all paths exist before trying to build
info:
	@echo "----------------------------------------------"
	@echo "STARCCM_INSTALL : $(STARCCM_INSTALL)"
	@echo "STARCCM_LIB_DIR : $(STARCCM_LIB_DIR)"
	@echo "STARCCM_SYS_DIR : $(STARCCM_SYS_DIR)"
	@echo "CXX             : $(CXX)"
	@echo "TARGET          : $(TARGET)"
	@echo "----------------------------------------------"
	@echo "Checking paths:"
	@test -d $(STARCCM_LIB_DIR) \
		&& echo "  [OK] STARCCM_LIB_DIR found" \
		|| echo "  [!!] STARCCM_LIB_DIR NOT FOUND"
	@test -f $(STARCCM_LIB_DIR)/libUserFunctions.so \
		&& echo "  [OK] libUserFunctions.so found" \
		|| echo "  [!!] libUserFunctions.so NOT FOUND"
	@test -d $(STARCCM_SYS_DIR) \
		&& echo "  [OK] STARCCM_SYS_DIR found" \
		|| echo "  [!!] STARCCM_SYS_DIR NOT FOUND"
	@test -f $(STARCCM_SYS_DIR)/libstdc++.so.6 \
		&& echo "  [OK] libstdc++.so.6 found" \
		|| echo "  [!!] libstdc++.so.6 NOT FOUND"
	@which $(CXX) > /dev/null 2>&1 \
		&& echo "  [OK] $(CXX) found at $$(which $(CXX))" \
		|| echo "  [!!] $(CXX) NOT found — try: module load clang"
	@echo "----------------------------------------------"

.PHONY: all clean info