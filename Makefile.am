include $(top_srcdir)/gtest.mk

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libsummer \
	docs \
	bindings

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsummer-1.0.pc

$(pkgconfig_DATA) : config.status

EXTRA_DIST = \
	libsummer-1.0.pc.in \
	libsummer-1.0-uninstalled.pc.in \
	gen-changelog.py

DISTCLEANFILES =

if USE_LCOV
coverage::
	mkdir -p ./coverage
	lcov --compat-libtool --directory . --zerocounters
	-$(MAKE) check
	lcov --compat-libtool --directory ./libsummer --capture --output-file ./coverage/$(PACKAGE)_tmp.info
	lcov --compat-libtool --extract ./coverage/$(PACKAGE)_tmp.info "$(PWD)/libsummer*" -o ./coverage/$(PACKAGE).info
	$(RM) ./coverage/$(PACKAGE)_tmp.info
	genhtml -o ./coverage --num-spaces 2 ./coverage/$(PACKAGE).info
endif

dist-hook:
	if test -d .git; then \
		python $(top_srcdir)/gen-changelog.py > $(distdir)/cl-t; \
		rm -f $(distdir)/ChangeLog; \
		mv $(distdir)/cl-t $(distdir)/ChangeLog; \
	fi
