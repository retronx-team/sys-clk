mkfile_path	:=	$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir	:=	$(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

LIBS		:=	-lglfw3 -lEGL -lglapi -ldrm_nouveau -lm $(LIBS)

SOURCES		:=	$(SOURCES) \
				$(current_dir)/lib \
				$(current_dir)/lib/extern/glad \
				$(current_dir)/lib/extern/nanovg \
				$(current_dir)/lib/extern/libretro-common/compat \
				$(current_dir)/lib/extern/libretro-common/encodings \
				$(current_dir)/lib/extern/libretro-common/features

INCLUDES	:=	$(INCLUDES) \
				$(current_dir)/include \
				$(current_dir)/include/borealis/extern/glad \
				$(current_dir)/include/borealis/extern/nanovg \
				$(current_dir)/include/borealis/extern/libretro-common
