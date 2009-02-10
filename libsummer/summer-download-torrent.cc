/* summer-download-torrent.cc */

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

#include "summer-download-torrent.hh"
#include "summer-download-web.h"
#include "summer-debug.h"
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <boost/filesystem.hpp>

/**
 * SECTION:summer-download-torrent
 * @short_description: Provides a way to download audio and video files via
 * bittorrent.
 * @stability: Unstable
 * @see_also: %SummerDownloadWeb
 *
 * This is the downloader for bittorrent files.
 */

/**
 * SummerDownloadTorrent:
 *
 * A %SummerDownload-based class for files that's available on bittorrent.
 */

static void summer_download_torrent_class_init (SummerDownloadTorrentClass *klass);
static void summer_download_torrent_init       (SummerDownloadTorrent *obj);
static void summer_download_torrent_finalize   (GObject *obj);

struct _SummerDownloadTorrentPrivate {
	libtorrent::torrent_handle handle;
	gfloat max_ratio;
};

#define SUMMER_DOWNLOAD_TORRENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                                SUMMER_TYPE_DOWNLOAD_TORRENT, \
                                                SummerDownloadTorrentPrivate))

static libtorrent::session *session;
static gint session_refs = 0;

static gushort default_min_port = 6881;
static gushort default_max_port = 6899;
static gint default_max_up_speed = -1;
static gfloat default_max_ratio = 5;

static GSList *downloads;

enum {
	PROP_0,
	PROP_PORT,
	PROP_MAX_UP_SPEED,
	PROP_FILENAME,
	PROP_MAX_RATIO
};

G_DEFINE_TYPE (SummerDownloadTorrent, summer_download_torrent, SUMMER_TYPE_DOWNLOAD);

static gboolean
check_done_seeding (gpointer data) {
	if (!SUMMER_IS_DOWNLOAD_TORRENT (data))
		return FALSE;
	SummerDownloadTorrent *self = SUMMER_DOWNLOAD_TORRENT (data);
	libtorrent::torrent_status status = self->priv->handle.status ();

	gchar *name;
	g_object_get (self, "filename", &name, NULL);
	
	gfloat ratio = status.all_time_upload / 
		(gfloat) (status.all_time_download > status.total_wanted_done ? 
			status.all_time_download : status.total_wanted_done);
	summer_debug ("%s: %i uploaded, %i downloaded. Ratio: %f", name, (gint) status.all_time_upload, (gint) status.all_time_download, ratio);
	g_free (name);
	
	if ((status.state == libtorrent::torrent_status::seeding ||
				status.state == libtorrent::torrent_status::finished) &&
			ratio > self->priv->max_ratio) {
		self->priv->handle.pause ();
		g_object_unref (G_OBJECT (self));
		return FALSE;
	}
	return TRUE;
}

static gboolean
check_progress (gpointer data) {
	if (!SUMMER_IS_DOWNLOAD_TORRENT (data))
		return FALSE;
	SummerDownload *self = SUMMER_DOWNLOAD (data);
	SummerDownloadTorrentPrivate *priv = SUMMER_DOWNLOAD_TORRENT (self)->priv;
	libtorrent::torrent_status status = priv->handle.status ();
	if (status.state == libtorrent::torrent_status::downloading) {
		g_signal_emit_by_name (self, "download-update", 
			(gint) status.total_wanted_done,
			(gint) status.total_wanted);
	}
	gchar *name;
	g_object_get (self, "filename", &name, NULL);
	g_free (name);
	if (status.state == libtorrent::torrent_status::finished
			|| status.state == libtorrent::torrent_status::seeding) {
		gchar *save_dir;
		g_object_get (self, "save-dir", &save_dir, NULL);
		boost::filesystem::path boost_save_dir (save_dir);
		g_free (save_dir);
		if (boost_save_dir != priv->handle.save_path ())
			priv->handle.move_storage (boost_save_dir);
		g_signal_emit_by_name (self, "download-complete", //FIXME: will give the wrong path if save-dir and temp-dir are on different drives
			priv->handle.save_path ().string ().c_str ());
		return FALSE;
	}
	return TRUE;
}

static void
on_metafile_downloaded (SummerDownloadWeb *dl, gchar *metafile_path, gpointer user_data)
{
	summer_debug ("%s: Metafile downloaded - torrent transfer starting", metafile_path);
	g_return_if_fail (SUMMER_IS_DOWNLOAD_TORRENT (user_data));
	g_return_if_fail (metafile_path != NULL);
	SummerDownload *self = SUMMER_DOWNLOAD (user_data);
	SummerDownloadTorrentPrivate *priv = SUMMER_DOWNLOAD_TORRENT (self)->priv;

	gchar *tmp_dir, *save_dir;
	g_object_get (self, "tmp-dir", &tmp_dir, "save-dir", &save_dir, NULL);

	libtorrent::add_torrent_params p;

	p.ti = new libtorrent::torrent_info (metafile_path);

	boost::filesystem::path fastresume (tmp_dir);
	fastresume /= p.ti->name () + ".fastresume";

	std::vector<char> resume;
	if (libtorrent::load_file (fastresume, resume) == 0) {
		summer_debug ("Loading fastresume from %s", fastresume.string().c_str());
		p.resume_data = &resume;
	}

	boost::filesystem::path completed_path (save_dir);
	completed_path /= p.ti->name ();

	if (!boost::filesystem::exists (completed_path)) {
		boost::filesystem::path output_dir (tmp_dir);
		p.save_path = output_dir;
	} else {
		summer_debug ("Torrent downloaded - seeding");
		p.save_path = boost::filesystem::path (save_dir);
	}
	g_free (tmp_dir);
	
	priv->handle = session->add_torrent (p);
	libtorrent::torrent_info info = priv->handle.get_torrent_info ();
	g_timeout_add_seconds (5, (GSourceFunc) check_progress, self);
	g_timeout_add_seconds (30, (GSourceFunc) check_done_seeding, self);
}

static void
start (SummerDownload *self)
{
	g_return_if_fail (SUMMER_IS_DOWNLOAD_TORRENT (self));
	gchar *url, *tmp_dir;
	g_object_get (self, "tmp-dir", &tmp_dir, "url", &url, NULL);
	gchar *metafile_dir = g_build_filename (tmp_dir, "metafiles", NULL);
	SummerDownload *web = summer_download_web_new (NULL, url);
	g_object_set (web, "save-dir", metafile_dir, NULL);
	g_free (tmp_dir);
	g_free (metafile_dir);
	g_free (url);

	g_signal_connect (web, 
		"download-complete", 
		G_CALLBACK (on_metafile_downloaded), 
		self);

	summer_download_start (web);
}

static void
set_property (GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	SummerDownloadTorrentPrivate *priv;
	priv = SUMMER_DOWNLOAD_TORRENT (object)->priv;

	switch (prop_id) {
	case PROP_MAX_UP_SPEED:
		session->set_upload_rate_limit (g_value_get_int (value));
		break;
	case PROP_MAX_RATIO:
		priv->max_ratio = g_value_get_float (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	SummerDownloadTorrentPrivate *priv;
	priv = SUMMER_DOWNLOAD_TORRENT (object)->priv;

	switch (prop_id) {
	case PROP_PORT:
		g_value_set_int (value, session->listen_port ());
		break;
	case PROP_MAX_UP_SPEED:
		g_value_set_int (value, session->upload_rate_limit ());
		break;
	case PROP_FILENAME:
		g_value_set_string (value, priv->handle.name ().c_str ());
		break;
	case PROP_MAX_RATIO:
		g_value_set_float (value, priv->max_ratio);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
summer_download_torrent_class_init (SummerDownloadTorrentClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	gobject_class->finalize = summer_download_torrent_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	SummerDownloadClass *download_class;
	download_class = SUMMER_DOWNLOAD_CLASS (klass);
	download_class->start = start;

	g_type_class_add_private (gobject_class, sizeof(SummerDownloadTorrentPrivate));

	GParamSpec *pspec;

	pspec = g_param_spec_int ("port",
		"Port",
		"The port used by the bittorrent backend",
		0,
		65535,
		6899,
		G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROP_PORT, pspec);

	/**
	 * SummerDownloadTorrent:max-up-speed:
	 *
	 * This is the maximum upload speed allowed, in kbps. Will be initialized 
	 * from the value set by %summer_set on construction. %-1 means no maximum.
	 */
	pspec = g_param_spec_int ("max-up-speed",
		"Maximum upload speed",
		"This is the maximum upload speed allowed",
		-1,
		G_MAXINT,
		-1,
		GParamFlags (G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_MAX_UP_SPEED, pspec);

	pspec = g_param_spec_float ("max-ratio",
		"Maximum ratio",
		"The ratio at wich a torrent should stop seeding.",
		1,
		G_MAXFLOAT,
		5,
		GParamFlags (G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, PROP_MAX_RATIO, pspec);

	g_object_class_override_property (gobject_class, PROP_FILENAME, "filename");
}

static void
summer_download_torrent_init (SummerDownloadTorrent *self)
{
	self->priv = SUMMER_DOWNLOAD_TORRENT_GET_PRIVATE(self);
	
	if (session_refs == 0) {
		session = new libtorrent::session ();
		session->listen_on (std::make_pair (default_min_port, default_max_port));
		summer_debug ("Listening on %u", session->listen_port ());
	}
	session_refs++;

	downloads = g_slist_prepend (downloads, self);

	self->priv->max_ratio = default_max_ratio;
}

static void
summer_download_torrent_finalize (GObject *obj)
{
	session_refs--;
	session->remove_torrent (SUMMER_DOWNLOAD_TORRENT (obj)->priv->handle);
	if (session_refs == 0)
		delete session;
	GSList *l;
	for (l = downloads; l != NULL; l = l->next) {
		if (l->data == obj) {
			downloads = g_slist_delete_link (downloads, l);
			break;
		}
	}
	G_OBJECT_CLASS(summer_download_torrent_parent_class)->finalize (obj);
}

/**
 * summer_download_torrent_new:
 * @mime: the mime typ of the file that's going to be downloaded. This should be
 * available in the feed.
 * @url: the URL of the file that's going to be downloaded.
 *
 * Creates a new #SummerDownloadTorrent
 *
 * You're not supposed to call this function directly - instead, use the
 * factory %summer_create_download, which will go through all the downloaders,
 * looking for one that's suitable.
 *
 * Returns: a newly created #SummerDownloadTorrent object if @mime and @url is
 * suitable, otherwise %NULL.
 */
SummerDownload*
summer_download_torrent_new (gchar *mime, gchar *url)
{
	if (!g_strcmp0(mime, "application/x-bittorrent")) {
		return SUMMER_DOWNLOAD (g_object_new (SUMMER_TYPE_DOWNLOAD_TORRENT, 
			"url", url,
			"max-up-speed", default_max_up_speed,
			NULL));
	}
	return NULL;
}

/**
 * summer_download_torrent_set_default():
 * @min_port: In the range of ports to try, this is the lower boundary.
 * @max_port: In the range of ports to try, this is the upper boundary.
 * @max_up_speed: The maximum upload speed to allow.
 * @max_ratio: The ratio at wich the torrent should stop seeding.
 *
 * Set default options for all new SummerDownloadTorrent objects.
 *
 * These options may be set at any time during the session. You may change these
 * as you wish after downloads have been started, but note that changing these
 * will only affect downloads started after the change. @max_up_speed can be
 * individually overridden by changing the %SummerDownloadTorrent::max-up-speed
 * property. The port selected in the range specified by @min_port and @max_port
 * can be retrieved by looking at the %SummerDownloadTorrent::port property.
 */
void
summer_download_torrent_set_default (
	gint min_port, 
	gint max_port, 
	gint max_up_speed,
	gfloat max_ratio)
{
	if (min_port != 0)
		default_min_port = min_port;
	if (max_port != 0)
		default_max_port = max_port;
	if (max_up_speed != 0)
		default_max_up_speed = max_up_speed;
	if (max_ratio != 0.0)
		default_max_ratio = max_ratio;
}

void
create_autoresume ()
{
	std::vector<libtorrent::torrent_handle> handles = session->get_torrents ();
	session->pause ();
	session->set_alert_mask (libtorrent::alert::storage_notification | libtorrent::alert::error_notification);
	int num_resume_data = 0;
	for (std::vector<libtorrent::torrent_handle>::iterator handle = handles.begin ();
			handle != handles.end (); handle++) {
		if (!handle->has_metadata ())
			continue;
		if (!handle->is_valid ())
			continue;

		handle->save_resume_data ();
		num_resume_data++;
	}

	while (num_resume_data > 0) {
		libtorrent::alert const *a = session->wait_for_alert (libtorrent::seconds(10));

		if (a == 0)
			break;

		std::auto_ptr<libtorrent::alert> holder = session->pop_alert ();

		if (dynamic_cast<libtorrent::save_resume_data_failed_alert const *> (a)) {
			summer_debug ("Couldn't save fastresume data: %s",
				dynamic_cast<libtorrent::save_resume_data_failed_alert const *> (a)->msg.c_str ());
			num_resume_data--;
			continue;
		}

		libtorrent::save_resume_data_alert const *rd = dynamic_cast<libtorrent::save_resume_data_alert const *> (a);

		if (rd == 0) {
			continue;
		}

		GSList *l;
		for (l = downloads; l != NULL; l = l->next) {
			if (SUMMER_DOWNLOAD_TORRENT (l->data)->priv->handle == rd->handle)
				break;
		}

		if (l == NULL) {
			summer_debug ("Auto-resume available, but nowhere to write it");
			num_resume_data--;
			continue;
		}
		gchar *tmp_dir;
		tmp_dir = summer_download_get_tmp_dir (SUMMER_DOWNLOAD (l->data));
		boost::filesystem::path out_path (tmp_dir);
		g_free (tmp_dir);
		out_path /= SUMMER_DOWNLOAD_TORRENT (l->data)->priv->handle.get_torrent_info ().name () + ".fastresume";
		summer_debug ("saving fastresume to %s", out_path.string ().c_str ());
		boost::filesystem::ofstream out (out_path, std::ios_base::binary);
		out.unsetf (std::ios_base::skipws);
		libtorrent::bencode (std::ostream_iterator<char> (out), *rd->resume_data);
		num_resume_data--;
	}
}

/**
 * summer_download_torrent_shutdown:
 *
 * Disconnect from the tracker.
 */
void
summer_download_torrent_shutdown ()
{
	if (session_refs > 0) {
		create_autoresume ();
		delete session;
		session_refs = 0;
	}
}
