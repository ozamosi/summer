/* summer-rss2-parser.h */

/* This file is part of libsummer.
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

#ifndef __SUMMER_RSS2_PARSER_H__
#define __SUMMER_RSS2_PARSER_H__

#include "summer.h"
/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define SUMMER_TYPE_RSS2_PARSER             (summer_rss2_parser_get_type())
#define SUMMER_RSS2_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_RSS2_PARSER,SummerRss2Parser))
#define SUMMER_RSS2_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_RSS2_PARSER,SummerRss2ParserClass))
#define SUMMER_IS_RSS2_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_RSS2_PARSER))
#define SUMMER_IS_RSS2_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_RSS2_PARSER))
#define SUMMER_RSS2_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_RSS2_PARSER,SummerRss2ParserClass))

typedef struct _SummerRss2Parser      SummerRss2Parser;
typedef struct _SummerRss2ParserClass SummerRss2ParserClass;

struct _SummerRss2Parser {
	 GObject parent;
	/* insert public members, if any */
};

struct _SummerRss2ParserClass {
	GObjectClass parent_class;
	/* insert signal callback declarations, eg. */
	/* void (* my_event) (SummerRss2Parser* obj); */
};

/* member functions */
GType        summer_rss2_parser_get_type    (void) G_GNUC_CONST;

/* typical parameter-less _new function */
/* if this is a kind of GtkWidget, it should probably return at GtkWidget* */
SummerRss2Parser*    summer_rss2_parser_new         (void);

/* fill in other public functions, eg.: */
/* 	void       summer_rss2_parser_do_something (SummerRss2Parser *self, const gchar* param); */
/* 	gboolean   summer_rss2_parser_has_foo      (SummerRss2Parser *self, gint value); */


G_END_DECLS

#endif /* __SUMMER_RSS2_PARSER_H__ */

