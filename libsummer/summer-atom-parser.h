/* summer-atom-parser.h */

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

#ifndef __SUMMER_ATOM_PARSER_H__
#define __SUMMER_ATOM_PARSER_H__

#include "summer-feed-parser.h"

G_BEGIN_DECLS

#define SUMMER_TYPE_ATOM_PARSER             (summer_atom_parser_get_type())
#define SUMMER_ATOM_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),SUMMER_TYPE_ATOM_PARSER,SummerAtomParser))
#define SUMMER_ATOM_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),SUMMER_TYPE_ATOM_PARSER,SummerAtomParserClass))
#define SUMMER_IS_ATOM_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),SUMMER_TYPE_ATOM_PARSER))
#define SUMMER_IS_ATOM_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),SUMMER_TYPE_ATOM_PARSER))
#define SUMMER_ATOM_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),SUMMER_TYPE_ATOM_PARSER,SummerAtomParserClass))

typedef struct _SummerAtomParser      SummerAtomParser;
typedef struct _SummerAtomParserClass SummerAtomParserClass;

struct _SummerAtomParser {
	 SummerFeedParser parent;
};

struct _SummerAtomParserClass {
	 SummerFeedParserClass parent_class;
};

GType        summer_atom_parser_get_type    (void) G_GNUC_CONST;

SummerAtomParser*    summer_atom_parser_new         (void);

G_END_DECLS

#endif /* __SUMMER_ATOM_PARSER_H__ */
