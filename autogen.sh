#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

echo $srcdir
PKG_NAME="libsummer"

. gnome-autogen.sh

#workaround http://bugzilla.gnome.org/show_bug.cgi?id=572396 for older gtk-doc
sed -e 's#) --mode=compile#) --tag=CC --mode=compile#' $srcdir/gtk-doc.make \
		> $srcdir/gtk-doc.temp \
	&& mv $srcdir/gtk-doc.temp $srcdir/gtk-doc.make
sed -e 's#) --mode=link#) --tag=CC --mode=link#' $srcdir/gtk-doc.make \
		> $srcdir/gtk-doc.temp \
	&& mv $srcdir/gtk-doc.temp $srcdir/gtk-doc.make
