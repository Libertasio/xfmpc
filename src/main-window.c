/*
 *  Copyright (c) 2009 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009 Vincent Legout <vincent@xfce.org>
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

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <mpdclient.h>
#include <preferences.h>
#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>


#define XFMPC_TYPE_MAIN_WINDOW (xfmpc_main_window_get_type ())
#define XFMPC_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindow))
#define XFMPC_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))
#define XFMPC_IS_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_IS_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_MAIN_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))

typedef struct _XfmpcMainWindow XfmpcMainWindow;
typedef struct _XfmpcMainWindowClass XfmpcMainWindowClass;
typedef struct _XfmpcMainWindowPrivate XfmpcMainWindowPrivate;

#define XFMPC_TYPE_INTERFACE (xfmpc_interface_get_type ())
#define XFMPC_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterface))
#define XFMPC_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))
#define XFMPC_IS_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_INTERFACE))
#define XFMPC_IS_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_INTERFACE))
#define XFMPC_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))

typedef struct _XfmpcInterface XfmpcInterface;
typedef struct _XfmpcInterfaceClass XfmpcInterfaceClass;

#define XFMPC_TYPE_STATUSBAR (xfmpc_statusbar_get_type ())
#define XFMPC_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbar))
#define XFMPC_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))
#define XFMPC_IS_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_STATUSBAR))
#define XFMPC_IS_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_STATUSBAR))
#define XFMPC_STATUSBAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))

typedef struct _XfmpcStatusbar XfmpcStatusbar;
typedef struct _XfmpcStatusbarClass XfmpcStatusbarClass;

#define XFMPC_TYPE_EXTENDED_INTERFACE (xfmpc_extended_interface_get_type ())
#define XFMPC_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterface))
#define XFMPC_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))
#define XFMPC_IS_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_IS_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_EXTENDED_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

typedef struct _XfmpcExtendedInterface XfmpcExtendedInterface;
typedef struct _XfmpcExtendedInterfaceClass XfmpcExtendedInterfaceClass;

struct _XfmpcMainWindow {
	GtkWindow parent_instance;
	XfmpcMainWindowPrivate * priv;
};

struct _XfmpcMainWindowClass {
	GtkWindowClass parent_class;
};

struct _XfmpcMainWindowPrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcPreferences* preferences;
	GtkVBox* vbox;
	GtkActionGroup* action_group;
	GtkWidget* statusbar;
	GtkUIManager* ui_manager;
};



GType xfmpc_main_window_get_type (void);
#define XFMPC_MAIN_WINDOW_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowPrivate))
enum  {
	XFMPC_MAIN_WINDOW_DUMMY_PROPERTY
};
static void xfmpc_main_window_action_statusbar (XfmpcMainWindow* self, GtkAction* action);
static void _xfmpc_main_window_action_statusbar_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_close (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_close_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_previous (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_previous_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_pp (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_pp_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_stop (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_stop_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_next (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_next_gtk_action_callback (GtkAction* action, gpointer self);
static void xfmpc_main_window_action_volume (XfmpcMainWindow* self);
static void _xfmpc_main_window_action_volume_gtk_action_callback (GtkAction* action, gpointer self);
#define XFMPC_MAIN_WINDOW_main_ui "<ui><accelerator action=\"quit\" /><accelerator action=\"previous\" /><accelerator action=\"pp\" /><accelerator action=\"stop\" /><accelerator action=\"next\" /><accelerator action=\"volume\" /></ui>"
#define XFMPC_MAIN_WINDOW_main_ui_length (gint) 187u
static gboolean xfmpc_main_window_cb_window_state_event (XfmpcMainWindow* self, const GdkEventWindowState* event);
static void xfmpc_main_window_close_window (XfmpcMainWindow* self);
static gboolean xfmpc_main_window_cb_window_closed (XfmpcMainWindow* self, const GdkEvent* event);
GType xfmpc_interface_get_type (void);
void xfmpc_interface_pp_clicked (XfmpcInterface* self);
void xfmpc_interface_popup_volume (XfmpcInterface* self);
XfmpcStatusbar* xfmpc_statusbar_new (void);
XfmpcStatusbar* xfmpc_statusbar_construct (GType object_type);
GType xfmpc_statusbar_get_type (void);
void xfmpc_statusbar_set_text (XfmpcStatusbar* self, const char* value);
static void xfmpc_main_window_update_statusbar (XfmpcMainWindow* self);
static void xfmpc_main_window_cb_playlist_changed (XfmpcMainWindow* self);
static void xfmpc_main_window_cb_show_statusbar_changed (XfmpcMainWindow* self, GParamSpec* pspec);
XfmpcMainWindow* xfmpc_main_window_new (void);
XfmpcMainWindow* xfmpc_main_window_construct (GType object_type);
XfmpcMainWindow* xfmpc_main_window_new (void);
static gboolean _xfmpc_main_window_cb_window_closed_gtk_widget_delete_event (XfmpcMainWindow* _sender, const GdkEvent* event, gpointer self);
static gboolean _xfmpc_main_window_cb_window_state_event_gtk_widget_window_state_event (XfmpcMainWindow* _sender, const GdkEventWindowState* event, gpointer self);
XfmpcInterface* xfmpc_interface_new (void);
XfmpcInterface* xfmpc_interface_construct (GType object_type);
XfmpcExtendedInterface* xfmpc_extended_interface_new (void);
XfmpcExtendedInterface* xfmpc_extended_interface_construct (GType object_type);
GType xfmpc_extended_interface_get_type (void);
static void _xfmpc_main_window_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_main_window_cb_show_statusbar_changed_xfmpc_preferences_notify (XfmpcPreferences* _sender, GParamSpec* pspec, gpointer self);
static GObject * xfmpc_main_window_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer xfmpc_main_window_parent_class = NULL;
static void xfmpc_main_window_finalize (GObject* obj);

static const GtkToggleActionEntry XFMPC_MAIN_WINDOW_toggle_action_entries[] = {{"view-statusbar", NULL, "", NULL, NULL, (GCallback) _xfmpc_main_window_action_statusbar_gtk_action_callback, FALSE}};
static const GtkActionEntry XFMPC_MAIN_WINDOW_action_entries[] = {{"quit", NULL, "", "<control>q", NULL, (GCallback) _xfmpc_main_window_action_close_gtk_action_callback}, {"previous", NULL, "", "<control>b", NULL, (GCallback) _xfmpc_main_window_action_previous_gtk_action_callback}, {"pp", NULL, "", "<control>p", NULL, (GCallback) _xfmpc_main_window_action_pp_gtk_action_callback}, {"stop", NULL, "", "<control>s", NULL, (GCallback) _xfmpc_main_window_action_stop_gtk_action_callback}, {"next", NULL, "", "<control>f", NULL, (GCallback) _xfmpc_main_window_action_next_gtk_action_callback}, {"volume", NULL, "", "<control>v", NULL, (GCallback) _xfmpc_main_window_action_volume_gtk_action_callback}};


static void _xfmpc_main_window_action_statusbar_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_statusbar (self, action);
}


static void _xfmpc_main_window_action_close_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_close (self);
}


static void _xfmpc_main_window_action_previous_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_previous (self);
}


static void _xfmpc_main_window_action_pp_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_pp (self);
}


static void _xfmpc_main_window_action_stop_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_stop (self);
}


static void _xfmpc_main_window_action_next_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_next (self);
}


static void _xfmpc_main_window_action_volume_gtk_action_callback (GtkAction* action, gpointer self) {
	xfmpc_main_window_action_volume (self);
}


static gboolean xfmpc_main_window_cb_window_state_event (XfmpcMainWindow* self, const GdkEventWindowState* event) {
	gboolean _tmp1_;
	g_return_val_if_fail (self != NULL, FALSE);
	if ((*event).type != GDK_WINDOW_STATE) {
		return FALSE;
	}
	_tmp1_ = FALSE;
	if (((gboolean) (*event).changed_mask) & GDK_WINDOW_STATE_STICKY) {
		gboolean _tmp2_;
		_tmp1_ = (g_object_get ((GtkWidget*) self, "visible", &_tmp2_, NULL), _tmp2_);
	} else {
		_tmp1_ = FALSE;
	}
	/**
	          * Hiding the top level window will unstick it too, and send a
	          * window-state-event signal, so here we take the value only if
	          * the window is visible
	          **/
	if (_tmp1_) {
		gboolean sticky;
		sticky = FALSE;
		if ((((gboolean) (*event).new_window_state) & GDK_WINDOW_STATE_STICKY) == FALSE) {
			sticky = FALSE;
		} else {
			sticky = TRUE;
		}
		xfmpc_preferences_set_last_window_state_sticky (self->priv->preferences, sticky);
	}
	return FALSE;
}


static gboolean xfmpc_main_window_cb_window_closed (XfmpcMainWindow* self, const GdkEvent* event) {
	g_return_val_if_fail (self != NULL, FALSE);
	xfmpc_main_window_close_window (self);
	return FALSE;
}


static void xfmpc_main_window_action_close (XfmpcMainWindow* self) {
	g_return_if_fail (self != NULL);
	xfmpc_main_window_close_window (self);
}


static void xfmpc_main_window_close_window (XfmpcMainWindow* self) {
	gint posx;
	gint posy;
	gint width;
	gint height;
	g_return_if_fail (self != NULL);
	posx = 0;
	posy = 0;
	width = 0;
	height = 0;
	gtk_window_get_position ((GtkWindow*) self, &posx, &posy);
	gtk_window_get_size ((GtkWindow*) self, &width, &height);
	xfmpc_preferences_set_last_window_posx (self->priv->preferences, posx);
	xfmpc_preferences_set_last_window_posy (self->priv->preferences, posy);
	xfmpc_preferences_set_last_window_width (self->priv->preferences, width);
	xfmpc_preferences_set_last_window_height (self->priv->preferences, height);
	gtk_main_quit ();
}


static void xfmpc_main_window_action_previous (XfmpcMainWindow* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_previous (self->priv->mpdclient);
}


static void xfmpc_main_window_action_pp (XfmpcMainWindow* self) {
	XfmpcInterface* _tmp0_;
	XfmpcInterface* interface;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	interface = (_tmp0_ = XFMPC_INTERFACE (g_object_get_data ((GObject*) self, "XfmpcInterface")), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	xfmpc_interface_pp_clicked (interface);
	(interface == NULL) ? NULL : (interface = (g_object_unref (interface), NULL));
}


static void xfmpc_main_window_action_stop (XfmpcMainWindow* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_stop (self->priv->mpdclient);
}


static void xfmpc_main_window_action_next (XfmpcMainWindow* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_next (self->priv->mpdclient);
}


static void xfmpc_main_window_action_volume (XfmpcMainWindow* self) {
	XfmpcInterface* _tmp0_;
	XfmpcInterface* interface;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	interface = (_tmp0_ = XFMPC_INTERFACE (g_object_get_data ((GObject*) self, "XfmpcInterface")), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	xfmpc_interface_popup_volume (interface);
	(interface == NULL) ? NULL : (interface = (g_object_unref (interface), NULL));
}


static void xfmpc_main_window_action_statusbar (XfmpcMainWindow* self, GtkAction* action) {
	gboolean active;
	gboolean _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (action != NULL);
	active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
	_tmp0_ = FALSE;
	if (!active) {
		_tmp0_ = self->priv->statusbar != NULL;
	} else {
		_tmp0_ = FALSE;
	}
	if (_tmp0_) {
		GtkWidget* _tmp1_;
		gtk_object_destroy ((GtkObject*) self->priv->statusbar);
		_tmp1_ = NULL;
		self->priv->statusbar = (_tmp1_ = NULL, (self->priv->statusbar == NULL) ? NULL : (self->priv->statusbar = (g_object_unref (self->priv->statusbar), NULL)), _tmp1_);
	} else {
		gboolean _tmp2_;
		_tmp2_ = FALSE;
		if (active) {
			_tmp2_ = self->priv->statusbar == NULL;
		} else {
			_tmp2_ = FALSE;
		}
		if (_tmp2_) {
			GtkWidget* _tmp3_;
			_tmp3_ = NULL;
			self->priv->statusbar = (_tmp3_ = (GtkWidget*) g_object_ref_sink (xfmpc_statusbar_new ()), (self->priv->statusbar == NULL) ? NULL : (self->priv->statusbar = (g_object_unref (self->priv->statusbar), NULL)), _tmp3_);
			gtk_widget_show (self->priv->statusbar);
			gtk_box_pack_start ((GtkBox*) self->priv->vbox, self->priv->statusbar, FALSE, FALSE, (guint) 0);
		}
	}
}


static void xfmpc_main_window_update_statusbar (XfmpcMainWindow* self) {
	gint seconds;
	gint length;
	GString* text;
	g_return_if_fail (self != NULL);
	seconds = 0;
	length = 0;
	if (self->priv->statusbar == NULL) {
		return;
	}
	if (!xfmpc_mpdclient_is_connected (self->priv->mpdclient)) {
		return;
	}
	length = xfmpc_mpdclient_playlist_get_length (self->priv->mpdclient);
	seconds = xfmpc_mpdclient_playlist_get_total_time (self->priv->mpdclient);
	text = g_string_new ("");
	if ((seconds / 3600) > 0) {
		g_string_append_printf (text, _ ("%d songs, %d hours and %d minutes"), length, seconds / 3600, (seconds / 60) % 60);
	} else {
		g_string_append_printf (text, _ ("%d songs, %d minutes"), length, (seconds / 60) % 60);
	}
	xfmpc_statusbar_set_text (XFMPC_STATUSBAR (self->priv->statusbar), text->str);
	(text == NULL) ? NULL : (text = (g_string_free (text, TRUE), NULL));
}


static void xfmpc_main_window_cb_playlist_changed (XfmpcMainWindow* self) {
	g_return_if_fail (self != NULL);
	xfmpc_main_window_update_statusbar (self);
}


static void xfmpc_main_window_cb_show_statusbar_changed (XfmpcMainWindow* self, GParamSpec* pspec) {
	GtkAction* _tmp0_;
	GtkAction* action;
	gboolean active;
	g_return_if_fail (self != NULL);
	g_return_if_fail (pspec != NULL);
	_tmp0_ = NULL;
	action = (_tmp0_ = gtk_action_group_get_action (self->priv->action_group, "view-statusbar"), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	active = xfmpc_preferences_get_show_statusbar (self->priv->preferences);
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), active);
	xfmpc_main_window_update_statusbar (self);
	(action == NULL) ? NULL : (action = (g_object_unref (action), NULL));
}


XfmpcMainWindow* xfmpc_main_window_construct (GType object_type) {
	XfmpcMainWindow * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcMainWindow* xfmpc_main_window_new (void) {
	return xfmpc_main_window_construct (XFMPC_TYPE_MAIN_WINDOW);
}


static gboolean _xfmpc_main_window_cb_window_closed_gtk_widget_delete_event (XfmpcMainWindow* _sender, const GdkEvent* event, gpointer self) {
	return xfmpc_main_window_cb_window_closed (self, event);
}


static gboolean _xfmpc_main_window_cb_window_state_event_gtk_widget_window_state_event (XfmpcMainWindow* _sender, const GdkEventWindowState* event, gpointer self) {
	return xfmpc_main_window_cb_window_state_event (self, event);
}


static void _xfmpc_main_window_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_main_window_cb_playlist_changed (self);
}


static void _xfmpc_main_window_cb_show_statusbar_changed_xfmpc_preferences_notify (XfmpcPreferences* _sender, GParamSpec* pspec, gpointer self) {
	xfmpc_main_window_cb_show_statusbar_changed (self, pspec);
}


static GObject * xfmpc_main_window_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcMainWindowClass * klass;
	GObjectClass * parent_class;
	XfmpcMainWindow * self;
	GError * _inner_error_;
	klass = XFMPC_MAIN_WINDOW_CLASS (g_type_class_peek (XFMPC_TYPE_MAIN_WINDOW));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_MAIN_WINDOW (obj);
	_inner_error_ = NULL;
	{
		GtkVBox* _tmp0_;
		gboolean _tmp1_;
		gboolean _tmp2_;
		XfmpcInterface* interface;
		GtkHSeparator* separator;
		XfmpcExtendedInterface* extended_interface;
		GtkUIManager* _tmp3_;
		GtkActionGroup* _tmp4_;
		GtkAccelGroup* _tmp5_;
		GtkAccelGroup* accel_group;
		self->priv->mpdclient = xfmpc_mpdclient_get ();
		self->priv->preferences = xfmpc_preferences_get ();
		/* Window */
		gtk_window_set_default_icon_name ("xfmpc");
		gtk_window_set_icon_name ((GtkWindow*) self, "stock_volume");
		gtk_window_set_title ((GtkWindow*) self, PACKAGE_NAME);
		gtk_window_set_default_size ((GtkWindow*) self, 330, 330);
		g_signal_connect_object ((GtkWidget*) self, "delete-event", (GCallback) _xfmpc_main_window_cb_window_closed_gtk_widget_delete_event, self, 0);
		g_signal_connect_object ((GtkWidget*) self, "window-state-event", (GCallback) _xfmpc_main_window_cb_window_state_event_gtk_widget_window_state_event, self, 0);
		_tmp0_ = NULL;
		self->priv->vbox = (_tmp0_ = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 0)), (self->priv->vbox == NULL) ? NULL : (self->priv->vbox = (g_object_unref (self->priv->vbox), NULL)), _tmp0_);
		gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->priv->vbox);
		_tmp1_ = FALSE;
		if (xfmpc_preferences_get_last_window_posx (self->priv->preferences) != (-1)) {
			_tmp1_ = xfmpc_preferences_get_last_window_posy (self->priv->preferences) != (-1);
		} else {
			_tmp1_ = FALSE;
		}
		if (_tmp1_) {
			gtk_window_move ((GtkWindow*) self, xfmpc_preferences_get_last_window_posx (self->priv->preferences), xfmpc_preferences_get_last_window_posy (self->priv->preferences));
		}
		_tmp2_ = FALSE;
		if (xfmpc_preferences_get_last_window_width (self->priv->preferences) != (-1)) {
			_tmp2_ = xfmpc_preferences_get_last_window_height (self->priv->preferences) != (-1);
		} else {
			_tmp2_ = FALSE;
		}
		if (_tmp2_) {
			gtk_window_set_default_size ((GtkWindow*) self, xfmpc_preferences_get_last_window_width (self->priv->preferences), xfmpc_preferences_get_last_window_height (self->priv->preferences));
		}
		if (xfmpc_preferences_get_last_window_state_sticky (self->priv->preferences) == TRUE) {
			gtk_window_stick ((GtkWindow*) self);
		}
		/* Interface */
		interface = g_object_ref_sink (xfmpc_interface_new ());
		g_object_set_data ((GObject*) self, "XfmpcInterface", interface);
		gtk_box_pack_start ((GtkBox*) self->priv->vbox, (GtkWidget*) interface, FALSE, FALSE, (guint) 4);
		/* Separator */
		separator = g_object_ref_sink ((GtkHSeparator*) gtk_hseparator_new ());
		gtk_box_pack_start ((GtkBox*) self->priv->vbox, (GtkWidget*) separator, FALSE, FALSE, (guint) 0);
		/* ExtendedInterface */
		extended_interface = g_object_ref_sink (xfmpc_extended_interface_new ());
		gtk_box_pack_start ((GtkBox*) self->priv->vbox, (GtkWidget*) extended_interface, TRUE, TRUE, (guint) 0);
		/* Accelerators */
		_tmp3_ = NULL;
		self->priv->ui_manager = (_tmp3_ = gtk_ui_manager_new (), (self->priv->ui_manager == NULL) ? NULL : (self->priv->ui_manager = (g_object_unref (self->priv->ui_manager), NULL)), _tmp3_);
		/* Action group */
		_tmp4_ = NULL;
		self->priv->action_group = (_tmp4_ = gtk_action_group_new ("XfmpcMainWindow"), (self->priv->action_group == NULL) ? NULL : (self->priv->action_group = (g_object_unref (self->priv->action_group), NULL)), _tmp4_);
		gtk_action_group_add_actions (self->priv->action_group, XFMPC_MAIN_WINDOW_action_entries, G_N_ELEMENTS (XFMPC_MAIN_WINDOW_action_entries), self);
		gtk_action_group_add_toggle_actions (self->priv->action_group, XFMPC_MAIN_WINDOW_toggle_action_entries, G_N_ELEMENTS (XFMPC_MAIN_WINDOW_toggle_action_entries), self);
		gtk_ui_manager_insert_action_group (self->priv->ui_manager, self->priv->action_group, 0);
		{
			gtk_ui_manager_add_ui_from_string (self->priv->ui_manager, XFMPC_MAIN_WINDOW_main_ui, (gssize) XFMPC_MAIN_WINDOW_main_ui_length, &_inner_error_);
			if (_inner_error_ != NULL) {
				goto __catch0_g_error;
				goto __finally0;
			}
		}
		goto __finally0;
		__catch0_g_error:
		{
			GError * e;
			e = _inner_error_;
			_inner_error_ = NULL;
			{
				g_warning (e->message);
				(e == NULL) ? NULL : (e = (g_error_free (e), NULL));
			}
		}
		__finally0:
		if (_inner_error_ != NULL) {
			(interface == NULL) ? NULL : (interface = (g_object_unref (interface), NULL));
			(separator == NULL) ? NULL : (separator = (g_object_unref (separator), NULL));
			(extended_interface == NULL) ? NULL : (extended_interface = (g_object_unref (extended_interface), NULL));
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
			g_clear_error (&_inner_error_);
		}
		/* Accel group */
		_tmp5_ = NULL;
		accel_group = (_tmp5_ = gtk_ui_manager_get_accel_group (self->priv->ui_manager), (_tmp5_ == NULL) ? NULL : g_object_ref (_tmp5_));
		gtk_window_add_accel_group ((GtkWindow*) self, accel_group);
		/* show-statusbar action */
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (gtk_action_group_get_action (self->priv->action_group, "view-statusbar")), xfmpc_preferences_get_show_statusbar (self->priv->preferences));
		/* === Signals === */
		g_signal_connect_object (self->priv->mpdclient, "playlist-changed", (GCallback) _xfmpc_main_window_cb_playlist_changed_xfmpc_mpdclient_playlist_changed, self, 0);
		g_signal_connect_object (self->priv->preferences, "notify::show-statusbar", (GCallback) _xfmpc_main_window_cb_show_statusbar_changed_xfmpc_preferences_notify, self, 0);
		(interface == NULL) ? NULL : (interface = (g_object_unref (interface), NULL));
		(separator == NULL) ? NULL : (separator = (g_object_unref (separator), NULL));
		(extended_interface == NULL) ? NULL : (extended_interface = (g_object_unref (extended_interface), NULL));
		(accel_group == NULL) ? NULL : (accel_group = (g_object_unref (accel_group), NULL));
	}
	return obj;
}


static void xfmpc_main_window_class_init (XfmpcMainWindowClass * klass) {
	xfmpc_main_window_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcMainWindowPrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_main_window_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_main_window_finalize;
}


static void xfmpc_main_window_instance_init (XfmpcMainWindow * self) {
	self->priv = XFMPC_MAIN_WINDOW_GET_PRIVATE (self);
}


static void xfmpc_main_window_finalize (GObject* obj) {
	XfmpcMainWindow * self;
	self = XFMPC_MAIN_WINDOW (obj);
	(self->priv->vbox == NULL) ? NULL : (self->priv->vbox = (g_object_unref (self->priv->vbox), NULL));
	(self->priv->action_group == NULL) ? NULL : (self->priv->action_group = (g_object_unref (self->priv->action_group), NULL));
	(self->priv->statusbar == NULL) ? NULL : (self->priv->statusbar = (g_object_unref (self->priv->statusbar), NULL));
	(self->priv->ui_manager == NULL) ? NULL : (self->priv->ui_manager = (g_object_unref (self->priv->ui_manager), NULL));
	G_OBJECT_CLASS (xfmpc_main_window_parent_class)->finalize (obj);
}


GType xfmpc_main_window_get_type (void) {
	static GType xfmpc_main_window_type_id = 0;
	if (xfmpc_main_window_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcMainWindowClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_main_window_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcMainWindow), 0, (GInstanceInitFunc) xfmpc_main_window_instance_init, NULL };
		xfmpc_main_window_type_id = g_type_register_static (GTK_TYPE_WINDOW, "XfmpcMainWindow", &g_define_type_info, 0);
	}
	return xfmpc_main_window_type_id;
}




