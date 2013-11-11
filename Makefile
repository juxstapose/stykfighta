#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'test'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wall

# define any directories containing header files other than /usr/include
INCLUDES = -I /usr/local/include/SDL2 -I /Users/juxstapose32/Projects/stykfighta

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS=

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lSDL2 -lSDL2_image -lm

# define the C source files
SRCS = util.c pf_list.c pathfinding.c behaviors.c darray.c sprite.c log.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = test

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:    $(MAIN)
	@echo  $(MAIN) has been compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

pf_list.o: /usr/include/stdio.h /usr/include/sys/cdefs.h
pf_list.o: /usr/include/sys/_symbol_aliasing.h
pf_list.o: /usr/include/sys/_posix_availability.h /usr/include/Availability.h
pf_list.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
pf_list.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
pf_list.o: /usr/include/i386/_types.h /usr/include/secure/_stdio.h
pf_list.o: /usr/include/secure/_common.h /usr/include/stdlib.h
pf_list.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
pf_list.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
pf_list.o: /usr/include/i386/signal.h /usr/include/i386/_structs.h
pf_list.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
pf_list.o: /usr/include/sys/resource.h /usr/include/machine/endian.h
pf_list.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
pf_list.o: /usr/include/libkern/_OSByteOrder.h
pf_list.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
pf_list.o: /usr/include/machine/types.h /usr/include/i386/types.h
pf_list.o: /usr/include/assert.h pf_list.h
pathfinding.o: /usr/include/stdlib.h /usr/include/Availability.h
pathfinding.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
pathfinding.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
pathfinding.o: /usr/include/sys/_symbol_aliasing.h
pathfinding.o: /usr/include/sys/_posix_availability.h
pathfinding.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
pathfinding.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
pathfinding.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
pathfinding.o: /usr/include/i386/signal.h /usr/include/i386/_structs.h
pathfinding.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
pathfinding.o: /usr/include/sys/resource.h /usr/include/machine/endian.h
pathfinding.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
pathfinding.o: /usr/include/libkern/_OSByteOrder.h
pathfinding.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
pathfinding.o: /usr/include/machine/types.h /usr/include/i386/types.h
pathfinding.o: /usr/include/stdio.h /usr/include/secure/_stdio.h
pathfinding.o: /usr/include/secure/_common.h /usr/include/assert.h
pathfinding.o: /usr/include/string.h /usr/include/strings.h
pathfinding.o: /usr/include/secure/_string.h pf_list.h pathfinding.h
behaviors.o: /usr/include/stdlib.h /usr/include/Availability.h
behaviors.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
behaviors.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
behaviors.o: /usr/include/sys/_symbol_aliasing.h
behaviors.o: /usr/include/sys/_posix_availability.h
behaviors.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
behaviors.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
behaviors.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
behaviors.o: /usr/include/i386/signal.h /usr/include/i386/_structs.h
behaviors.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
behaviors.o: /usr/include/sys/resource.h /usr/include/machine/endian.h
behaviors.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
behaviors.o: /usr/include/libkern/_OSByteOrder.h
behaviors.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
behaviors.o: /usr/include/machine/types.h /usr/include/i386/types.h
behaviors.o: /usr/include/stdio.h /usr/include/secure/_stdio.h
behaviors.o: /usr/include/secure/_common.h /usr/include/time.h
behaviors.o: /usr/include/_structs.h /usr/include/math.h
behaviors.o: /usr/include/architecture/i386/math.h main.h pf_list.h
behaviors.o: pathfinding.h behaviors.h
darray.o: /usr/include/stdlib.h /usr/include/Availability.h
darray.o: /usr/include/AvailabilityInternal.h /usr/include/_types.h
darray.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
darray.o: /usr/include/sys/_symbol_aliasing.h
darray.o: /usr/include/sys/_posix_availability.h
darray.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
darray.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
darray.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
darray.o: /usr/include/i386/signal.h /usr/include/i386/_structs.h
darray.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
darray.o: /usr/include/sys/resource.h /usr/include/machine/endian.h
darray.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
darray.o: /usr/include/libkern/_OSByteOrder.h
darray.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
darray.o: /usr/include/machine/types.h /usr/include/i386/types.h
darray.o: /usr/include/stdio.h /usr/include/secure/_stdio.h
darray.o: /usr/include/secure/_common.h /usr/include/assert.h darray.h
sprite.o: /usr/local/include/SDL2/SDL.h /usr/local/include/SDL2/SDL_main.h
sprite.o: /usr/local/include/SDL2/SDL_stdinc.h
sprite.o: /usr/local/include/SDL2/SDL_config.h
sprite.o: /usr/local/include/SDL2/SDL_platform.h
sprite.o: /usr/local/include/SDL2/begin_code.h
sprite.o: /usr/local/include/SDL2/close_code.h /usr/include/sys/types.h
sprite.o: /usr/include/sys/appleapiopts.h /usr/include/sys/cdefs.h
sprite.o: /usr/include/sys/_symbol_aliasing.h
sprite.o: /usr/include/sys/_posix_availability.h /usr/include/machine/types.h
sprite.o: /usr/include/i386/types.h /usr/include/i386/_types.h
sprite.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
sprite.o: /usr/include/machine/endian.h /usr/include/i386/endian.h
sprite.o: /usr/include/sys/_endian.h /usr/include/libkern/_OSByteOrder.h
sprite.o: /usr/include/libkern/i386/_OSByteOrder.h
sprite.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
sprite.o: /usr/include/i386/_structs.h /usr/include/stdio.h
sprite.o: /usr/include/Availability.h /usr/include/AvailabilityInternal.h
sprite.o: /usr/include/_types.h /usr/include/secure/_stdio.h
sprite.o: /usr/include/secure/_common.h /usr/include/stdlib.h
sprite.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
sprite.o: /usr/include/machine/signal.h /usr/include/i386/signal.h
sprite.o: /usr/include/sys/resource.h /usr/include/alloca.h
sprite.o: /usr/include/stddef.h /usr/include/string.h /usr/include/strings.h
sprite.o: /usr/include/secure/_string.h /usr/include/inttypes.h
sprite.o: /usr/include/stdint.h /usr/include/ctype.h /usr/include/runetype.h
sprite.o: /usr/include/math.h /usr/include/architecture/i386/math.h
sprite.o: /usr/include/iconv.h /usr/local/include/SDL2/SDL_assert.h
sprite.o: /usr/local/include/SDL2/SDL_atomic.h
sprite.o: /usr/local/include/SDL2/SDL_audio.h
sprite.o: /usr/local/include/SDL2/SDL_error.h
sprite.o: /usr/local/include/SDL2/SDL_endian.h
sprite.o: /usr/local/include/SDL2/SDL_mutex.h
sprite.o: /usr/local/include/SDL2/SDL_thread.h
sprite.o: /usr/local/include/SDL2/SDL_rwops.h
sprite.o: /usr/local/include/SDL2/SDL_clipboard.h
sprite.o: /usr/local/include/SDL2/SDL_cpuinfo.h
sprite.o: /usr/local/include/SDL2/SDL_events.h
sprite.o: /usr/local/include/SDL2/SDL_video.h
sprite.o: /usr/local/include/SDL2/SDL_pixels.h
sprite.o: /usr/local/include/SDL2/SDL_rect.h
sprite.o: /usr/local/include/SDL2/SDL_surface.h
sprite.o: /usr/local/include/SDL2/SDL_blendmode.h
sprite.o: /usr/local/include/SDL2/SDL_keyboard.h
sprite.o: /usr/local/include/SDL2/SDL_keycode.h
sprite.o: /usr/local/include/SDL2/SDL_scancode.h
sprite.o: /usr/local/include/SDL2/SDL_mouse.h
sprite.o: /usr/local/include/SDL2/SDL_joystick.h
sprite.o: /usr/local/include/SDL2/SDL_gamecontroller.h
sprite.o: /usr/local/include/SDL2/SDL_quit.h
sprite.o: /usr/local/include/SDL2/SDL_gesture.h
sprite.o: /usr/local/include/SDL2/SDL_touch.h
sprite.o: /usr/local/include/SDL2/SDL_haptic.h
sprite.o: /usr/local/include/SDL2/SDL_hints.h
sprite.o: /usr/local/include/SDL2/SDL_loadso.h
sprite.o: /usr/local/include/SDL2/SDL_log.h
sprite.o: /usr/local/include/SDL2/SDL_messagebox.h
sprite.o: /usr/local/include/SDL2/SDL_power.h
sprite.o: /usr/local/include/SDL2/SDL_render.h
sprite.o: /usr/local/include/SDL2/SDL_system.h
sprite.o: /usr/local/include/SDL2/SDL_timer.h
sprite.o: /usr/local/include/SDL2/SDL_version.h
sprite.o: /usr/local/include/SDL2/SDL_image.h /usr/include/assert.h main.h
sprite.o: util.h darray.h sprite.h pf_list.h pathfinding.h behaviors.h
