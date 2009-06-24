<?xml version="1.0"?>
<api version="1.0">
	<namespace name="Summer">
		<function name="create_download" symbol="summer_create_download">
			<return-type type="SummerDownload*"/>
			<parameters>
				<parameter name="item" type="SummerItemData*"/>
			</parameters>
		</function>
		<function name="debug" symbol="summer_debug">
			<return-type type="gboolean"/>
			<parameters>
				<parameter name="string" type="gchar*"/>
			</parameters>
		</function>
		<function name="libsummer_create_downloader" symbol="libsummer_create_downloader">
			<return-type type="LibsummerDownloader*"/>
			<parameters>
				<parameter name="url" type="guchar*"/>
				<parameter name="mime" type="guchar*"/>
			</parameters>
		</function>
		<function name="libsummer_create_feed_downloader" symbol="libsummer_create_feed_downloader">
			<return-type type="LibsummerFeedDownloader*"/>
			<parameters>
				<parameter name="url" type="gchar*"/>
			</parameters>
		</function>
		<function name="libsummer_init" symbol="libsummer_init">
			<return-type type="void"/>
		</function>
		<function name="shutdown" symbol="summer_shutdown">
			<return-type type="void"/>
		</function>
		<struct name="LibsummerDownloader">
			<method name="can_download" symbol="libsummer_downloader_can_download">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="url" type="guchar*"/>
					<parameter name="mime" type="guchar*"/>
				</parameters>
			</method>
			<method name="new" symbol="libsummer_downloader_new">
				<return-type type="LibsummerDownloader*"/>
			</method>
			<method name="run" symbol="libsummer_downloader_run">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="LibsummerDownloader*"/>
				</parameters>
			</method>
			<field name="parent" type="GObject"/>
		</struct>
		<struct name="LibsummerDownloaderClass">
			<field name="parent_class" type="GObjectClass"/>
			<field name="download_update" type="GCallback"/>
			<field name="download_complete" type="GCallback"/>
			<field name="run" type="GCallback"/>
		</struct>
		<struct name="LibsummerDownloaderWeb">
			<method name="can_download" symbol="libsummer_downloader_web_can_download">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="url" type="guchar*"/>
					<parameter name="mime" type="guchar*"/>
				</parameters>
			</method>
			<method name="new" symbol="libsummer_downloader_web_new">
				<return-type type="LibsummerDownloader*"/>
				<parameters>
					<parameter name="url" type="guchar*"/>
				</parameters>
			</method>
			<field name="parent" type="LibsummerDownloader"/>
		</struct>
		<struct name="LibsummerDownloaderWebClass">
			<field name="parent_class" type="LibsummerDownloaderClass"/>
		</struct>
		<struct name="LibsummerFeedDownloader">
			<method name="new" symbol="libsummer_feed_downloader_new">
				<return-type type="LibsummerFeedDownloader*"/>
			</method>
			<method name="run" symbol="libsummer_feed_downloader_run">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="LibsummerFeedDownloader*"/>
				</parameters>
			</method>
			<field name="parent" type="GObject"/>
		</struct>
		<struct name="LibsummerFeedDownloaderClass">
			<field name="parent_class" type="GObjectClass"/>
			<field name="feed_downloaded" type="GCallback"/>
			<field name="run" type="GCallback"/>
		</struct>
		<struct name="LibsummerFeedDownloaderRss">
			<method name="can_download" symbol="libsummer_feed_downloader_rss_can_download">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</method>
			<method name="new" symbol="libsummer_feed_downloader_rss_new">
				<return-type type="LibsummerFeedDownloader*"/>
				<parameters>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</method>
			<field name="parent" type="LibsummerFeedDownloader"/>
		</struct>
		<struct name="LibsummerFeedDownloaderRssClass">
			<field name="parent_class" type="LibsummerFeedDownloaderClass"/>
		</struct>
		<struct name="LibsummerFeedEntry">
			<field name="name" type="guchar*"/>
			<field name="description" type="guchar*"/>
			<field name="mime" type="guchar*"/>
			<field name="url" type="guchar*"/>
			<field name="size" type="gint"/>
		</struct>
		<struct name="LibsummerWebBackend">
			<method name="new" symbol="libsummer_web_backend_new">
				<return-type type="LibsummerWebBackend*"/>
			</method>
			<method name="run" symbol="libsummer_web_backend_run">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="LibsummerWebBackend*"/>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</method>
			<field name="parent" type="GObject"/>
		</struct>
		<struct name="LibsummerWebBackendClass">
			<field name="parent_class" type="GObjectClass"/>
			<field name="complete" type="GCallback"/>
			<field name="chunk" type="GCallback"/>
		</struct>
		<struct name="LibsummerWebData">
			<field name="buffer" type="gchar*"/>
			<field name="uri" type="gchar*"/>
			<field name="length" type="goffset"/>
		</struct>
		<struct name="SummerFeedRss">
			<method name="new" symbol="summer_feed_rss_new">
				<return-type type="SummerFeedRss*"/>
			</method>
			<field name="parent" type="GObject"/>
		</struct>
		<struct name="SummerFeedRssClass">
			<field name="parent_class" type="GObjectClass"/>
		</struct>
		<struct name="SummerXmlBackend">
			<method name="new" symbol="summer_xml_backend_new">
				<return-type type="SummerXmlBackend*"/>
			</method>
			<field name="parent" type="GObject"/>
		</struct>
		<struct name="SummerXmlBackendClass">
			<field name="parent_class" type="GObjectClass"/>
		</struct>
		<enum name="SummerDownloadState">
			<member name="SUMMER_DOWNLOAD_STATE_UNKNOWN" value="10"/>
			<member name="SUMMER_DOWNLOAD_STATE_DOWNLOADING" value="20"/>
			<member name="SUMMER_DOWNLOAD_STATE_FAILED" value="30"/>
			<member name="SUMMER_DOWNLOAD_STATE_SEEDING" value="40"/>
			<member name="SUMMER_DOWNLOAD_STATE_DONE" value="50"/>
		</enum>
		<object name="SummerAtomParser" parent="SummerFeedParser" type-name="SummerAtomParser" get-type="summer_atom_parser_get_type">
			<constructor name="new" symbol="summer_atom_parser_new">
				<return-type type="SummerAtomParser*"/>
			</constructor>
		</object>
		<object name="SummerDownload" parent="GObject" type-name="SummerDownload" get-type="summer_download_get_type">
			<method name="abort" symbol="summer_download_abort">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="delete" symbol="summer_download_delete">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
					<parameter name="error" type="GError**"/>
				</parameters>
			</method>
			<method name="error_quark" symbol="summer_download_error_quark">
				<return-type type="GQuark"/>
			</method>
			<method name="get_filename" symbol="summer_download_get_filename">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="get_save_dir" symbol="summer_download_get_save_dir">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="get_save_path" symbol="summer_download_get_save_path">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="get_state" symbol="summer_download_get_state">
				<return-type type="SummerDownloadState"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="get_tmp_dir" symbol="summer_download_get_tmp_dir">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="is_paused" symbol="summer_download_is_paused">
				<return-type type="gboolean"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</method>
			<method name="set_default" symbol="summer_download_set_default">
				<return-type type="void"/>
				<parameters>
					<parameter name="tmp_dir" type="gchar*"/>
					<parameter name="save_dir" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_filename" symbol="summer_download_set_filename">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
					<parameter name="filename" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_paused" symbol="summer_download_set_paused">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
					<parameter name="pause" type="gboolean"/>
				</parameters>
			</method>
			<method name="set_save_dir" symbol="summer_download_set_save_dir">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
					<parameter name="save_dir" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_tmp_dir" symbol="summer_download_set_tmp_dir">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
					<parameter name="tmp_dir" type="gchar*"/>
				</parameters>
			</method>
			<method name="start" symbol="summer_download_start">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
				</parameters>
			</method>
			<property name="downloadable" type="SummerDownloadableData*" readable="1" writable="1" construct="0" construct-only="1"/>
			<property name="filename" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="item" type="SummerItemData*" readable="1" writable="1" construct="0" construct-only="1"/>
			<property name="paused" type="gboolean" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="save-dir" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="state" type="SummerDownloadState" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="tmp-dir" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="url" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<signal name="download-complete" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
				</parameters>
			</signal>
			<signal name="download-done" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
				</parameters>
			</signal>
			<signal name="download-error" when="CLEANUP">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
					<parameter name="error" type="gpointer"/>
				</parameters>
			</signal>
			<signal name="download-started" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
				</parameters>
			</signal>
			<signal name="download-update" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerDownload*"/>
					<parameter name="received" type="guint64"/>
					<parameter name="length" type="guint64"/>
				</parameters>
			</signal>
			<vfunc name="abort">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</vfunc>
			<vfunc name="start">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerDownload*"/>
				</parameters>
			</vfunc>
		</object>
		<object name="SummerDownloadTorrent" parent="SummerDownload" type-name="SummerDownloadTorrent" get-type="summer_download_torrent_get_type">
			<constructor name="new" symbol="summer_download_torrent_new">
				<return-type type="SummerDownload*"/>
				<parameters>
					<parameter name="item" type="SummerItemData*"/>
				</parameters>
			</constructor>
			<method name="set_default" symbol="summer_download_torrent_set_default">
				<return-type type="void"/>
				<parameters>
					<parameter name="min_port" type="gint"/>
					<parameter name="max_port" type="gint"/>
					<parameter name="max_up_speed" type="gint"/>
					<parameter name="max_ratio" type="gfloat"/>
				</parameters>
			</method>
			<method name="shutdown" symbol="summer_download_torrent_shutdown">
				<return-type type="void"/>
			</method>
			<property name="max-ratio" type="gfloat" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="max-up-speed" type="gint" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="port" type="gint" readable="1" writable="0" construct="0" construct-only="0"/>
		</object>
		<object name="SummerDownloadWeb" parent="SummerDownload" type-name="SummerDownloadWeb" get-type="summer_download_web_get_type">
			<constructor name="new" symbol="summer_download_web_new">
				<return-type type="SummerDownload*"/>
				<parameters>
					<parameter name="item" type="SummerItemData*"/>
				</parameters>
			</constructor>
		</object>
		<object name="SummerDownloadYoutube" parent="SummerDownload" type-name="SummerDownloadYoutube" get-type="summer_download_youtube_get_type">
			<constructor name="new" symbol="summer_download_youtube_new">
				<return-type type="SummerDownload*"/>
				<parameters>
					<parameter name="item" type="SummerItemData*"/>
				</parameters>
			</constructor>
		</object>
		<object name="SummerDownloadableData" parent="GObject" type-name="SummerDownloadableData" get-type="summer_downloadable_data_get_type">
			<method name="get_length" symbol="summer_downloadable_data_get_length">
				<return-type type="guint64"/>
				<parameters>
					<parameter name="self" type="SummerDownloadableData*"/>
				</parameters>
			</method>
			<method name="get_mime" symbol="summer_downloadable_data_get_mime">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownloadableData*"/>
				</parameters>
			</method>
			<method name="get_url" symbol="summer_downloadable_data_get_url">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerDownloadableData*"/>
				</parameters>
			</method>
			<field name="item" type="SummerItemData*"/>
			<field name="url" type="gchar*"/>
			<field name="mime" type="gchar*"/>
			<field name="length" type="guint64"/>
		</object>
		<object name="SummerFeed" parent="GObject" type-name="SummerFeed" get-type="summer_feed_get_type">
			<method name="get_author" symbol="summer_feed_get_author">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_cache_dir" symbol="summer_feed_get_cache_dir">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_description" symbol="summer_feed_get_description">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_frequency" symbol="summer_feed_get_frequency">
				<return-type type="gint"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_id" symbol="summer_feed_get_id">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_items" symbol="summer_feed_get_items">
				<return-type type="GList*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_title" symbol="summer_feed_get_title">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_updated" symbol="summer_feed_get_updated">
				<return-type type="time_t"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_url" symbol="summer_feed_get_url">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="get_web_url" symbol="summer_feed_get_web_url">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="summer_feed_new">
				<return-type type="SummerFeed*"/>
			</constructor>
			<method name="peek_items" symbol="summer_feed_peek_items">
				<return-type type="GList*"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
				</parameters>
			</method>
			<method name="set_cache_dir" symbol="summer_feed_set_cache_dir">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
					<parameter name="cache_dir" type="gchar*"/>
				</parameters>
			</method>
			<method name="set_default" symbol="summer_feed_set_default">
				<return-type type="void"/>
				<parameters>
					<parameter name="cache_dir" type="gchar*"/>
					<parameter name="frequency" type="gint"/>
				</parameters>
			</method>
			<method name="set_frequency" symbol="summer_feed_set_frequency">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
					<parameter name="frequency" type="gint"/>
				</parameters>
			</method>
			<method name="set_url" symbol="summer_feed_set_url">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</method>
			<method name="shutdown" symbol="summer_feed_shutdown">
				<return-type type="void"/>
			</method>
			<method name="start" symbol="summer_feed_start">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeed*"/>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</method>
			<property name="author" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="cache-dir" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="description" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="frequency" type="gint" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="id" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="items" type="gpointer" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="title" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="updated" type="glong" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="url" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
			<property name="web-url" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<signal name="new-entries" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerFeed*"/>
				</parameters>
			</signal>
		</object>
		<object name="SummerFeedCache" parent="GObject" type-name="SummerFeedCache" get-type="summer_feed_cache_get_type">
			<method name="add_new_item" symbol="summer_feed_cache_add_new_item">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeedCache*"/>
					<parameter name="item" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="filter_old_items" symbol="summer_feed_cache_filter_old_items">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerFeedCache*"/>
					<parameter name="items" type="GList**"/>
				</parameters>
			</method>
			<method name="get" symbol="summer_feed_cache_get">
				<return-type type="SummerFeedCache*"/>
			</method>
			<property name="cache-file" type="char*" readable="1" writable="1" construct="0" construct-only="0"/>
		</object>
		<object name="SummerFeedData" parent="GObject" type-name="SummerFeedData" get-type="summer_feed_data_get_type">
			<method name="append_item" symbol="summer_feed_data_append_item">
				<return-type type="SummerItemData*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_author" symbol="summer_feed_data_get_author">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_description" symbol="summer_feed_data_get_description">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_id" symbol="summer_feed_data_get_id">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_items" symbol="summer_feed_data_get_items">
				<return-type type="GList*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_title" symbol="summer_feed_data_get_title">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_updated" symbol="summer_feed_data_get_updated">
				<return-type type="time_t"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<method name="get_web_url" symbol="summer_feed_data_get_web_url">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerFeedData*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="summer_feed_data_new">
				<return-type type="SummerFeedData*"/>
			</constructor>
			<field name="url" type="gchar*"/>
			<field name="title" type="gchar*"/>
			<field name="description" type="gchar*"/>
			<field name="id" type="gchar*"/>
			<field name="web_url" type="gchar*"/>
			<field name="author" type="gchar*"/>
			<field name="updated" type="time_t"/>
			<field name="items" type="GList*"/>
		</object>
		<object name="SummerFeedParser" parent="GObject" type-name="SummerFeedParser" get-type="summer_feed_parser_get_type">
			<method name="handle_node" symbol="summer_feed_parser_handle_node">
				<return-type type="gint"/>
				<parameters>
					<parameter name="self" type="SummerFeedParser*"/>
					<parameter name="node" type="xmlTextReader*"/>
					<parameter name="feed" type="SummerFeedData*"/>
					<parameter name="is_item" type="gboolean*"/>
				</parameters>
			</method>
			<method name="parse" symbol="summer_feed_parser_parse">
				<return-type type="SummerFeedData*"/>
				<parameters>
					<parameter name="parsers" type="SummerFeedParser**"/>
					<parameter name="num_parsers" type="int"/>
					<parameter name="reader" type="xmlTextReader*"/>
				</parameters>
			</method>
			<vfunc name="handle_node">
				<return-type type="gint"/>
				<parameters>
					<parameter name="self" type="SummerFeedParser*"/>
					<parameter name="node" type="xmlTextReader*"/>
					<parameter name="feed" type="SummerFeedData*"/>
					<parameter name="is_item" type="gboolean*"/>
				</parameters>
			</vfunc>
		</object>
		<object name="SummerItemData" parent="GObject" type-name="SummerItemData" get-type="summer_item_data_get_type">
			<method name="append_downloadable" symbol="summer_item_data_append_downloadable">
				<return-type type="SummerDownloadableData*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
					<parameter name="url" type="gchar*"/>
					<parameter name="mime" type="gchar*"/>
					<parameter name="length" type="guint64"/>
				</parameters>
			</method>
			<method name="get_author" symbol="summer_item_data_get_author">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_description" symbol="summer_item_data_get_description">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_downloadables" symbol="summer_item_data_get_downloadables">
				<return-type type="GList*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_id" symbol="summer_item_data_get_id">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_title" symbol="summer_item_data_get_title">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_updated" symbol="summer_item_data_get_updated">
				<return-type type="time_t"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<method name="get_web_url" symbol="summer_item_data_get_web_url">
				<return-type type="gchar*"/>
				<parameters>
					<parameter name="self" type="SummerItemData*"/>
				</parameters>
			</method>
			<field name="feed" type="SummerFeedData*"/>
			<field name="title" type="gchar*"/>
			<field name="description" type="gchar*"/>
			<field name="id" type="gchar*"/>
			<field name="web_url" type="gchar*"/>
			<field name="author" type="gchar*"/>
			<field name="updated" type="time_t"/>
			<field name="downloadables" type="GList*"/>
		</object>
		<object name="SummerRss2Parser" parent="SummerFeedParser" type-name="SummerRss2Parser" get-type="summer_rss2_parser_get_type">
			<constructor name="new" symbol="summer_rss2_parser_new">
				<return-type type="SummerRss2Parser*"/>
			</constructor>
		</object>
		<object name="SummerWebBackend" parent="GObject" type-name="SummerWebBackend" get-type="summer_web_backend_get_type">
			<method name="abort" symbol="summer_web_backend_abort">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerWebBackend*"/>
				</parameters>
			</method>
			<method name="fetch" symbol="summer_web_backend_fetch">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerWebBackend*"/>
				</parameters>
			</method>
			<method name="fetch_head" symbol="summer_web_backend_fetch_head">
				<return-type type="void"/>
				<parameters>
					<parameter name="self" type="SummerWebBackend*"/>
				</parameters>
			</method>
			<constructor name="new" symbol="summer_web_backend_new">
				<return-type type="SummerWebBackend*"/>
				<parameters>
					<parameter name="save_dir" type="gchar*"/>
					<parameter name="url" type="gchar*"/>
				</parameters>
			</constructor>
			<property name="filename" type="char*" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="length" type="guint64" readable="1" writable="0" construct="0" construct-only="0"/>
			<property name="save-dir" type="char*" readable="1" writable="1" construct="0" construct-only="1"/>
			<property name="url" type="char*" readable="1" writable="1" construct="1" construct-only="0"/>
			<signal name="download-chunk" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerWebBackend*"/>
					<parameter name="received" type="guint64"/>
					<parameter name="length" type="guint64"/>
				</parameters>
			</signal>
			<signal name="download-complete" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerWebBackend*"/>
					<parameter name="save_path" type="char*"/>
					<parameter name="save_data" type="char*"/>
				</parameters>
			</signal>
			<signal name="headers-parsed" when="FIRST">
				<return-type type="void"/>
				<parameters>
					<parameter name="obj" type="SummerWebBackend*"/>
				</parameters>
			</signal>
		</object>
	</namespace>
</api>
