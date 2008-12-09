/* summer-feed.h */

/* This file is part of summer_feed.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
 * 
 * Libsummer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the license, or (at your option) any later version.
 * 
 * Libsummer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Palace - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __SUMMER_FEED_H__
#define __SUMMER_FEED_H__

#include <glib-object.h>
/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define SUMMER_TYPE_FEED             (summer_feed_get_type())
#define SUMMER_FEED(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_FEED,SummerFeed))
#define SUMMER_FEED_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_FEED,SummerFeedClass))
#define SUMMER_IS_FEED(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_FEED))
#define SUMMER_IS_FEED_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_FEED))
#define SUMMER_FEED_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_FEED,SummerFeedClass))

typedef struct _SummerFeed      SummerFeed;
typedef struct _SummerFeedClass SummerFeedClass;

struct _SummerFeed {
	 GObject parent;
	/* insert public members, if any */
};

struct _SummerFeedClass {
	GObjectClass parent_class;
	/* insert signal callback declarations, eg. */
	/* void (* my_event) (SummerFeed* obj); */
};

/* member functions */
GType        summer_feed_get_type    (void) G_GNUC_CONST;

/* typical parameter-less _new function */
/* if this is a kind of GtkWidget, it should probably return at GtkWidget* */
SummerFeed*    summer_feed_new         (void);

/* fill in other public functions, eg.: */
/* 	void       summer_feed_do_something (SummerFeed *self, const gchar* param); */
/* 	gboolean   summer_feed_has_foo      (SummerFeed *self, gint value); */


G_END_DECLS

#endif /* __SUMMER_FEED_H__ */

