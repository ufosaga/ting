ifeq ($(ting_init_included),)
ting_init_included := true

include ./os.mk

ting_obj_dir := obj


#TODO: clear this_* variables

endif
