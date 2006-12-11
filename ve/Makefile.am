## Process this file with automake to produce makefile.in
SUBDIRS=.

INCLUDES = \
 	-I.					\
	-I..					\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	$(VICIOUS_CFLAGS)

# Use these only really for checking for deprecated use, not all the time
#	-DG_DISABLE_DEPRECATED			\
#	-DGDK_DISABLE_DEPRECATED		\
#	-DGDK_PIXBUF_DISABLE_DEPRECATED		\
#	-DGTK_DISABLE_DEPRECATED		\
#	-DGNOME_DISABLE_DEPRECATED		\
#

noinst_LIBRARIES = \
	@VICIOUS_UI_LIBS@ \
	libvicious.a

EXTRA_LIBRARIES = \
	libviciousui.a	

noinst_PROGRAMS = test-ve-config

libvicious_a_SOURCES =	\
	ve-i18n.h	\
	ve-misc.c	\
	ve-misc.h	\
	ve-config.c	\
	ve-config.h	\
	ve-nongnome.c	\
	vicious.h

libviciousui_a_SOURCES = \
	$(libvicious_a_SOURCES) \
	glade-helper.c	\
	glade-helper.h	\
	ve-miscui.c	\
	ve-miscui.h	\
	viciousui.h

test_ve_config_SOURCES = \
	test-ve-config.c

test_ve_config_LDADD = \
	$(VICIOUS_LIBS)				\
	$(INTLLIBS)				\
	$(GLIB_LIBS)				\
	-L$(top_builddir)/ve			\
	-lvicious