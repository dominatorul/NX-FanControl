# Define build targets
TARGETS := lib/libfancontrol overlay sysmodule
OUT_DIR := out

.PHONY: all build-only $(TARGETS) out clean autoclean

# Default target - auto clean before building
all: autoclean build-only

# Auto clean - removes old builds before starting new one
autoclean:
	@echo "Auto-cleaning previous build..."
	@rm -rf $(OUT_DIR)
	@$(MAKE) -C lib/libfancontrol clean 2>/dev/null || true
	@$(MAKE) -C overlay clean 2>/dev/null || true
	@$(MAKE) -C sysmodule clean 2>/dev/null || true
	@echo "Clean complete, starting fresh build..."
	@echo ""

# Build without cleaning (use 'make build-only' to skip auto-clean)
build-only: $(TARGETS) out

# Build each subproject
lib/libfancontrol:
	@echo "Building libfancontrol..."
	@$(MAKE) -C $@

overlay: lib/libfancontrol
	@echo "Building overlay..."
	@$(MAKE) -C $@

sysmodule: lib/libfancontrol
	@echo "Building sysmodule..."
	@$(MAKE) -C $@

# Handle output and packaging
out:
	@echo ""
	@echo "Packaging overlay..."
	@mkdir -p $(OUT_DIR)/switch/.overlays
	@cp ./overlay/NX-FanControl.ovl $(OUT_DIR)/switch/.overlays/Nx-FanControl.ovl

	@echo "Packaging sysmodule..."
	@mkdir -p $(OUT_DIR)/atmosphere/contents/00FF0000B378D640/flags
	@cp ./sysmodule/sysmodule.nsp $(OUT_DIR)/atmosphere/contents/00FF0000B378D640/exefs.nsp
	@cp ./sysmodule/toolbox.json $(OUT_DIR)/atmosphere/contents/00FF0000B378D640/
	@touch $(OUT_DIR)/atmosphere/contents/00FF0000B378D640/flags/boot2.flag
	
	@echo ""
	@echo "Build complete! Files are in '$(OUT_DIR)' directory"

# Manual clean command (use 'make clean' to only clean without building)
clean:
	@echo "Cleaning up build outputs..."
	@rm -rf $(OUT_DIR)
	@$(MAKE) -C lib/libfancontrol clean 2>/dev/null || true
	@$(MAKE) -C overlay clean 2>/dev/null || true
	@$(MAKE) -C sysmodule clean 2>/dev/null || true
	@echo "Clean complete!"

# Quick rebuild - clean and build specific target
rebuild-lib: 
	@$(MAKE) -C lib/libfancontrol clean 2>/dev/null || true
	@$(MAKE) lib/libfancontrol

rebuild-overlay:
	@$(MAKE) -C overlay clean 2>/dev/null || true
	@$(MAKE) overlay

rebuild-sysmodule:
	@$(MAKE) -C sysmodule clean 2>/dev/null || true
	@$(MAKE) sysmodule

# Help target
help:
	@echo "NX-FanControl Build System"
	@echo "=========================="
	@echo "Available targets:"
	@echo "  make          - Auto-clean and build everything (default)"
	@echo "  make build-only - Build without cleaning"
	@echo "  make clean    - Clean all build outputs"
	@echo "  make rebuild-lib - Clean and rebuild library only"
	@echo "  make rebuild-overlay - Clean and rebuild overlay only"
	@echo "  make rebuild-sysmodule - Clean and rebuild sysmodule only"
	@echo "  make help     - Show this help message"