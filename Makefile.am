V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

include $(top_srcdir)/gtest.mk

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsummer-1.0.pc

$(pkgconfig_DATA) : config.status

EXTRA_DIST = \
	libsummer-1.0.pc.in \
	libsummer-1.0-uninstalled.pc.in \
	gen-changelog.py

DISTCLEANFILES = \
	doltlibtool \
	doltcompile

if USE_LCOV
coverage::
	mkdir -p ./coverage
	lcov --compat-libtool --directory . --zerocounters
	-$(MAKE) test
	lcov --compat-libtool --directory . --capture --output-file ./coverage/$(PACKAGE)_tmp.info
	lcov --compat-libtool --extract ./coverage/$(PACKAGE)_tmp.info "*libsummer*" -o ./coverage/$(PACKAGE).info
	$(RM) ./coverage/$(PACKAGE)_tmp.info
	genhtml -o ./coverage --num-spaces 2 ./coverage/$(PACKAGE).info
endif

dist-hook:
	if test -d .git; then \
		python $(top_srcdir)/gen-changelog.py > $(distdir)/cl-t; \
		rm -f $(distdir)/ChangeLog; \
		mv $(distdir)/cl-t $(distdir)/ChangeLog; \
	fi

INCLUDES = \
	$(CFLAGS) \
	$(SUMMER_CFLAGS) \
	$(DISABLE_DEPRECATED) \
	-include $(CONFIG_HEADER) \
	-Wall -Wextra \
	-Wno-missing-field-initializers -Wno-unused-parameter \
	-Werror -O0 -g

CLEANFILES =

clean-local:
	rm -rf coverage
	rm -f *.gcda
	rm -f *.gcno

include libsummer.mk
include libsummer-tests.mk

if ENABLE_VALA
include vala.mk
endif

CLEANFILES += $(BUILT_SOURCES)

SUBDIRS = . docs/reference/summer
if ENABLE_PYTHON
SUBDIRS += bindings/python
endif
