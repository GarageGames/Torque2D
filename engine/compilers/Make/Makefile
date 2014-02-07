DEPS :=
LIB_TARGETS :=
LIB_TARGETS_DEBUG :=
SHARED_LIB_TARGETS :=
SHARED_LIB_TARGETS_DEBUG :=
APP_TARGETS :=
APP_TARGETS_DEBUG :=

all: debug release

clean:
	rm -rfv Debug
	rm -rfv Release
	rm -rfv lib

.PHONY: all debug release clean

-include x Torque2D
-include x zlib
-include x lpng
-include x ljpeg

release: $(LIB_TARGETS) $(SHARED_LIB_TARGETS) $(APP_TARGETS)
	@echo Built libraries: $(LIB_TARGETS)
	@echo Built shared libraries: $(SHARED_LIB_TARGETS)
	@echo Built apps: $(APP_TARGETS)

debug: $(LIB_TARGETS_DEBUG) $(SHARED_LIB_TARGETS_DEBUG) $(APP_TARGETS_DEBUG)
	@echo Built libraries: $(LIB_TARGETS_DEBUG)
	@echo Built shared libraries: $(SHARED_LIB_TARGETS_DEBUG)
	@echo Built apps: $(APP_TARGETS_DEBUG)
