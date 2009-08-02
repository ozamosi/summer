/* summer-transfer-speed.c */

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

#include "summer-transfer-speed.h"

#define MEASURE_TIME 5
/**
 * SECTION:summer-transfer-speed
 * @short_description: Utility for tracking transfer speeds
 * @stability: Private
 *
 * This object lets you track transfer speeds. It works by letting you add a
 * number of datapoints with #summer_transfer_speed_add_datapoint, and then
 * lets you retrieve the average speed for the last few data points by calling
 * #summer_transfer_speed_get.
 *
 * The object works with circular buffers: as you add more data points, the
 * oldest ones will be overwritten. You should thus not call
 * #summer_transfer_speed_add_datapoint too often, or the average will be
 * calculated over a very short time period, making the result unreliable.
 *
 * Note that #SummerTransferSpeed is not reference counted, and all object must
 * thus manually be destroyed with #summer_transfer_speed_destroy.
 */

/**
 * SummerTransferSpeed:
 *
 * This object lets you track transfer speeds. New objects are created with
 * #summer_transfer_speed_new, and old ones must be destroyed with
 * #summer_transfer_speed_delete.
 */
struct _SummerTransferSpeed {
	guint64 progress[MEASURE_TIME];
	GTimeVal timeval[MEASURE_TIME];
	gshort position;
};

/**
 * summer_transfer_speed_add_datapoint:
 * @self: A #SummerTransferSpeed object
 * @bits: The current progress
 *
 * Call this whenever you want to store a new datapoint, but not too often, or
 * the result will be calculated over a very short period of time: roughly once
 * per second should be enough.
 */
void
summer_transfer_speed_add_datapoint (SummerTransferSpeed *self, guint64 bits)
{
	/* The timestamp will be slightly too late, but we assume it's roughly
	 * equally late all the time
	 */
	self->position = (self->position + 1) % MEASURE_TIME;
	self->progress[self->position] = bits;
	g_get_current_time (&self->timeval[self->position]);
}

/**
 * summer_transfer_speed_get:
 * @self: A #SummerTransferSpeed object
 *
 * Retrieves the average transfer speed over the last few calls to
 * #summer_transfer_speed_add_datapoint. The speed will be in $unit per second,
 * where $unit is the unit you used for the input data.
 *
 * Returns: The transfer speed
 */
gfloat
summer_transfer_speed_get (SummerTransferSpeed *self)
{
//	gshort curr = (self->position + MEASURE_TIME - 1) % MEASURE_TIME;
	guint64 curr_pos = self->progress[self->position];
	GTimeVal curr_time = self->timeval[self->position];
	if (curr_time.tv_sec == 0)
		return 0.0;

	gshort oldest = (self->position + 1) % MEASURE_TIME;
	/* should not become an infinite loop: all positions cannot be null since
	 * curr_time isn't */
	while (self->timeval[oldest].tv_sec == 0) {
		oldest = (oldest + 1) % MEASURE_TIME;
	}
	/* if only one datapoint, assume it was collected during 1 second */
	if (oldest == self->position)
		return curr_pos;
	GTimeVal old_time = self->timeval[oldest];
	guint64 old_pos = self->progress[oldest];

	curr_time.tv_sec -= old_time.tv_sec;
	g_time_val_add (&curr_time, -old_time.tv_usec);
	return (curr_pos - old_pos) / (curr_time.tv_sec + curr_time.tv_usec / 1000000.0);
}

/**
 * summer_transfer_speed_new:
 *
 * Creates a new #SummerTransferSpeed object.
 *
 * Returns: The new #SummerTransferSpeed object
 */
SummerTransferSpeed*
summer_transfer_speed_new ()
{
	return g_slice_new0 (SummerTransferSpeed);
}

/**
 * summer_transfer_speed_destroy:
 * @self: A #SummerTransferSpeed object
 *
 * Destroys a #SummerTransferSpeed object, frees all memory associated with it,
 * and sets the #SummerTransferSpeed pointer in the argument to %NULL.
 */
void
summer_transfer_speed_destroy (SummerTransferSpeed **self)
{
	g_slice_free (SummerTransferSpeed, *self);
	*self = NULL;
}
