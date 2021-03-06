/*
 *  Copyright (c) 2009-2010 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009-2010 Vincent Legout <vincent@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

using Gtk;

namespace Xfmpc {

	public class Interface : Box {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private Gtk.Button button_prev;
		private Gtk.Button button_pp;
		private Gtk.Button button_next;
		private Gtk.VolumeButton button_volume;
		private Xfmpc.ProgressBar progress_bar;
		private Gtk.Label title;
		private Gtk.Label subtitle;

		private bool progress_bar_sync = true;

		construct {
			this.mpdclient = Xfmpc.Mpdclient.get_default ();
			this.preferences = Xfmpc.Preferences.get_default ();

			set_orientation (Gtk.Orientation.VERTICAL);
			set_border_width (4);

			var image = new Gtk.Image.from_icon_name ("media-skip-backward", Gtk.IconSize.BUTTON);
			this.button_prev = new Gtk.Button ();
			this.button_prev.set_relief (Gtk.ReliefStyle.NONE);
			this.button_prev.add (image);
			this.button_prev.get_style_context ().add_class ("primary-button");

			image = new Gtk.Image.from_icon_name ("media-playback-start", Gtk.IconSize.BUTTON);
			this.button_pp = new Gtk.Button ();
			this.button_pp.set_relief (Gtk.ReliefStyle.NONE);
			this.button_pp.add (image);
			this.button_pp.get_style_context ().add_class ("primary-button");

			image = new Gtk.Image.from_icon_name ("media-skip-forward", Gtk.IconSize.BUTTON);
			this.button_next = new Gtk.Button ();
			this.button_next.set_relief (Gtk.ReliefStyle.NONE);
			this.button_next.add (image);
			this.button_next.get_style_context ().add_class ("primary-button");

			this.button_volume = new Gtk.VolumeButton ();
			this.button_volume.set_relief (Gtk.ReliefStyle.NONE);
			this.button_volume.get_style_context ().add_class ("primary-button");
			var adjustment = button_volume.get_adjustment ();
			adjustment.upper *= 100;
			adjustment.step_increment *= 100;
			adjustment.page_increment *= 100;

			var progress_box = new Gtk.EventBox ();
			progress_bar = new Xfmpc.ProgressBar ();
			progress_bar.set_text ("0:00 / 0:00");
			progress_bar.set_fraction (1.0);
			progress_box.add (progress_bar);
			progress_box.set_above_child (true);

  	  	  	/* Title */
			var attrs = new Pango.AttrList ();
			var attr = Pango.attr_weight_new (Pango.Weight.BOLD);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr.copy ());

			attr = Pango.attr_scale_new ((double) Pango.Scale.X_LARGE);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr.copy ());

			title = new Gtk.Label (_("Not connected"));
			title.set_attributes (attrs);
			title.set_selectable (true);
			title.set_ellipsize (Pango.EllipsizeMode.END);
			title.xalign = 0.0f;
			title.yalign = 0.5f;

  	  	  	/* Subtitle */
			attrs = new Pango.AttrList ();
			attr = Pango.attr_scale_new ((double) Pango.Scale.SMALL);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr.copy ());

			this.subtitle = new Gtk.Label (Config.PACKAGE_STRING);
			this.subtitle.set_attributes (attrs);
			this.subtitle.set_selectable (true);
			this.subtitle.set_ellipsize (Pango.EllipsizeMode.END);
			this.subtitle.xalign = 0.0f;
			this.subtitle.yalign = 0.5f;

  	  	  	/* === Containers === */
			var box = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			pack_start (box, false, false, 0);
			box.pack_start (this.button_prev, false, false, 0);
			box.pack_start (this.button_pp, false, false, 0);
			box.pack_start (this.button_next, false, false, 0);
			box.pack_start (progress_box, true, true, 4);
			box.pack_start (this.button_volume, false, false, 0);

			var vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 0);
			pack_start (vbox, false, true, 0);
			vbox.add (this.title);
			vbox.add (this.subtitle);

  	  	  	/* === Signals === */
			this.button_prev.clicked.connect (cb_mpdclient_previous);
			this.button_pp.clicked.connect (pp_clicked);
			this.button_next.clicked.connect (cb_mpdclient_next);
			this.button_volume.value_changed.connect (volume_changed);
			progress_box.motion_notify_event.connect (cb_progress_box_motion_event);
			progress_box.button_press_event.connect (cb_progress_box_press_event);
			progress_box.button_release_event.connect (cb_progress_box_release_event);

			this.mpdclient.song_changed.connect (cb_song_changed);
			this.mpdclient.pp_changed.connect (cb_pp_changed);
			this.mpdclient.time_changed.connect (cb_time_changed);
			this.mpdclient.total_time_changed.connect (cb_total_time_changed);
			this.mpdclient.volume_changed.connect (cb_volume_changed);
			this.mpdclient.playlist_changed.connect (cb_playlist_changed);
			this.mpdclient.stopped.connect (cb_stopped);
		}

		public void set_title (string title) {
			this.title.set_text (title);
		}

		public void set_subtitle (string subtitle) {
			this.subtitle.set_text (subtitle);
		}

		public void pp_clicked () {
			if (!this.mpdclient.pp ())
				return;
			set_pp (this.mpdclient.is_playing ());
		}

		public void set_pp (bool play) {
			var image = (Gtk.Image) this.button_pp.get_child ();

			if (play == true)
				image.set_from_icon_name ("media-playback-pause", Gtk.IconSize.BUTTON);
			else
				image.set_from_icon_name ("media-playback-start", Gtk.IconSize.BUTTON);
		}

		private bool cb_progress_box_motion_event (Gdk.EventMotion event) {
			/* This event is started only after a press-event signal,
			 * we use it to update the progress bar with the position
			 * of the mouse. */
			int time_total = this.mpdclient.get_total_time ();
			if (time_total < 0)
				return false;

			Gtk.Allocation allocation;
			this.progress_bar.get_allocation (out allocation);
			double song_time = event.x / allocation.width;
			song_time *= time_total;

			if (song_time < 0)
				song_time = 0;
			else if (song_time > time_total)
				song_time = time_total;

			set_time ((int)song_time, time_total);
			return false;
		}

		private bool cb_progress_box_press_event (Gdk.EventButton event) {
			/* Block MPD signals from updating the progress bar */
			this.progress_bar_sync = false;
			return false;
		}

		private bool cb_progress_box_release_event (Gdk.EventButton event) {
			if (event.type != Gdk.EventType.BUTTON_RELEASE || event.button != 1)
				return false;

			/* Unblock MPD signals to update the progress bar */
			this.progress_bar_sync = true;

			int time_total = this.mpdclient.get_total_time ();
			if (time_total < 0)
				return false;

			Gtk.Allocation allocation;
			this.progress_bar.get_allocation (out allocation);
			double song_time = event.x / allocation.width;
			song_time *= time_total;

			this.mpdclient.set_song_time ((int) song_time);

			return true;
		}

		public void volume_changed (double value) {
			this.mpdclient.set_volume ((char) value);
		}

		public void set_volume (int volume) {
			this.button_volume.set_value (volume);
		}

		public void popup_volume () {
			GLib.Signal.emit_by_name (this.button_volume, "popup", null);
		}

		public void set_time (int song_time, int time_total) {
			int min, sec, min_total, sec_total;
			double fraction = 1.0;

			min = song_time / 60;
			sec = song_time % 60;

  	  	  	min_total = time_total / 60;
  	  	  	sec_total = time_total % 60;

			GLib.StringBuilder text = new GLib.StringBuilder ();
			text.append_printf ("%d:%02d / %d:%02d", min, sec, min_total, sec_total);
			this.progress_bar.set_text (text.str);

			if (time_total > 0)
				fraction = (float)song_time / (float)time_total;

			this.progress_bar.set_fraction ((fraction <= 1.0) ? fraction : 1.0);
		}

		public void reset () {
			set_pp (false);
			set_time (0, 0);
			set_volume (0);
			update_title ();
		}

		public void update_title () {
			if (this.mpdclient.is_playing ()) {
				set_title (this.mpdclient.get_title ());
				/*
				// write private function in case it is wished to avoid the
				// "n/a" values, but no big deal IMO
				text = get_subtitle (interface);
				 */
				/* TRANSLATORS: subtitle "by \"artist\" from \"album\" (year)" */
				string text = _("by \"%s\" from \"%s\" (%s)").printf (
						this.mpdclient.get_artist (),
						this.mpdclient.get_album (),
						this.mpdclient.get_date ());
				set_subtitle (text);
			}
			else if (this.mpdclient.is_stopped ()) {
				set_title (_("Stopped"));
				set_subtitle (Config.PACKAGE_STRING);
			}
			else if (!this.mpdclient.is_connected ()) {
				set_title (_("Not connected"));
				set_subtitle (Config.PACKAGE_STRING);
			}
		}

		private void cb_song_changed () {
  	  	  	update_title ();
		}

		private void cb_pp_changed (bool is_playing) {
			set_pp (is_playing);
			cb_song_changed ();
		}

		private void cb_time_changed (int song_time) {
			if (this.progress_bar_sync == false)
				return;
			set_time (song_time, mpdclient.get_total_time());
		}

		private void cb_total_time_changed (int total_time) {
			if (this.progress_bar_sync == false)
				return;
			set_time (mpdclient.get_time(), total_time);
		}

		private void cb_volume_changed (int volume) {
			set_volume (volume);
		}

		private void cb_playlist_changed () {
			update_title ();
		}

		private void cb_stopped () {
			set_pp (false);
			update_title ();
		}

		private void cb_mpdclient_previous () {
			this.mpdclient.previous ();
		}

		private void cb_mpdclient_next () {
			this.mpdclient.next ();
		}
	}
}
