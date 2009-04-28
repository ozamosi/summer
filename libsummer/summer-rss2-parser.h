/* summer-rss2-parser.h */

/* This file is part of libsummer.
 * Copyright © 2008 Robin Sonefors <ozamosi@flukkost.nu>
 * 
 * Libsummer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation.
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

//#include "summer.h"
#include "summer-feed-parser.h"
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
	 SummerFeedParser parent;
};

struct _SummerRss2ParserClass {
	SummerFeedParserClass parent_class;
};

GType        summer_rss2_parser_get_type    (void) G_GNUC_CONST;

SummerRss2Parser*    summer_rss2_parser_new         (void);

G_END_DECLS

#endif /* __SUMMER_RSS2_PARSER_H__ */
