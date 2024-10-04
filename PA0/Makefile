build_dir = build
target = HelloWorld
sources = helloworld.c
objects = $(patsubst %.c, $(build_dir)/%.o, $(sources))

CC = gcc
compiler_flags := -g -O0 -std=c99 -Wall

########### Compiler and Linker flags
CFLAGS += $(compiler_flags)

LDFLAGS += -lm

##### source file search path
# VPATH = src:../headers:fold1
# or
# vpath %.c PA0 	https://www.gnu.org/software/make/manual/make.html#General-Search

######### default target
all: $(build_dir) $(build_dir)/$(target)

$(build_dir):  
	@if [ -d $(build_dir) ]; then \
	 	echo "Remake the build directory."; \
		rm -rf $(build_dir); \
	fi; \
	mkdir -p $(build_dir); \

####### Link all files
$(build_dir)/$(target): $(objects)
	$(CC) $(CFLAGS) -o $@ $(objects)

###### Dependency files.
$(build_dir)/%.d: %.c | $(build_dir)
	@set -e; \
	rm -rf $@;	\
	$(CC) -M $(CFLAGS) $< > $@.$$$$;	\
	sed 's,\($*\)\.o[ :]*,\1.o $(notdir $@): ,g' < $@.$$$$$ > $@;	\
	rm -f $@.$$$$;

-include $(patsubst %.c, $(build_dir)/%.d, $(sources))
################ Generate Rules
# helloworld.o: HelloWorld.c # GNU make can auto inference the command

################# Static Pattern Rules
# https://www.gnu.org/software/make/manual/make.html#Static-Usage
# $(objects):%.o:%.c
# 	$(CC) -c $(CFLAGS) $< -o $(build_dir)/$@

$(build_dir)/%.o: %.c  | $(build_dir)
	$(CC) -c $(CFLAGS) $< -o $@


################ Automatic variables
# https://www.gnu.org/software/make/manual/make.html#Automatic-Variables


.PHONY: all  clean

clean:
	-rm -rf  $(build_dir)
