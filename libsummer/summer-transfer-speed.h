/* summer-transfer-speed.h */

/* This file is part of libsummer.
 * Copyright © 2008-2009 Robin Sonefors <ozamosi@flukkost.nu>
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

#ifndef __SUMMER_TRANSFER_SPEED_H__
#define __SUMMER_TRANSFER_SPEED_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _SummerTransferSpeed SummerTransferSpeed;

void summer_transfer_speed_add_datapoint (SummerTransferSpeed *self, guint64 bits);
gfloat summer_transfer_speed_get (SummerTransferSpeed *self);

SummerTransferSpeed* summer_transfer_speed_new ();
void summer_transfer_speed_destroy (SummerTransferSpeed **self);

G_END_DECLS

#endif /* __SUMMER_TRANSFER_SPEED_H__ */
