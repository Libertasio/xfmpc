/*
 *  Copyright (c) 2008 Mike Massonnet <mmassonnet@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4util/libxfce4util.h>
#include <libmpd/libmpd.h>

#include "mpdclient.h"

#define MPD_HOST "localhost"
#define MPD_PORT 6600

#define XFMPC_MPDCLIENT_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_MPDCLIENT, XfmpcMpdclientPrivate))



enum
{
  SIG_SONG_CHANGED,
  SIG_PP_CHANGED,
  SIG_TIME_CHANGED,
  SIG_VOLUME_CHANGED,
  SIG_STOPPED,
  LAST_SIGNAL
};

static guint            xfmpc_mpdclient_signals[LAST_SIGNAL] = { 0 };



static void             xfmpc_mpdclient_class_init              (XfmpcMpdclientClass *klass);
static void             xfmpc_mpdclient_init                    (XfmpcMpdclient *mpdclient);
static void             xfmpc_mpdclient_finalize                (GObject *object);

static void             xfmpc_mpdclient_initenv                 (XfmpcMpdclient *mpdclient);

static void             cb_xfmpc_mpdclient_status_changed       (MpdObj *mi,
                                                                 ChangedStatusType what,
                                                                 gpointer user_data);



struct _XfmpcMpdclientClass
{
  GObjectClass              parent_class;

  void (*song_changed)      (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*pp_changed)        (XfmpcMpdclient *mpdclient, gboolean is_playing, gpointer user_data);
  void (*time_changed)      (XfmpcMpdclient *mpdclient, gint time, gint total_time, gpointer user_data);
  void (*volume_changed)    (XfmpcMpdclient *mpdclient, gint volume, gpointer user_data);
  void (*stopped)           (XfmpcMpdclient *mpdclient, gpointer user_data);
};

struct _XfmpcMpdclient
{
  GObject                   parent;
  XfmpcMpdclientPrivate    *priv;
};

struct _XfmpcMpdclientPrivate
{
  MpdObj                   *mi;
  gchar                    *host;
  guint                     port;
  gchar                    *passwd;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_mpdclient_get_type ()
{
  static GType xfmpc_mpdclient_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_mpdclient_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_mpdclient_info =
        {
          sizeof (XfmpcMpdclientClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_mpdclient_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcMpdclient),
          0,
          (GInstanceInitFunc) xfmpc_mpdclient_init,
          NULL
        };
      xfmpc_mpdclient_type = g_type_register_static (G_TYPE_OBJECT, "XfmpcMpdclient", &xfmpc_mpdclient_info, 0);
    }

  return xfmpc_mpdclient_type;
}



static void
xfmpc_mpdclient_class_init (XfmpcMpdclientClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcMpdclientPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfmpc_mpdclient_finalize;

  xfmpc_mpdclient_signals[SIG_SONG_CHANGED] =
    g_signal_new ("song-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, song_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  xfmpc_mpdclient_signals[SIG_PP_CHANGED] =
    g_signal_new ("pp-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, pp_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);

  xfmpc_mpdclient_signals[SIG_TIME_CHANGED] =
    g_signal_new ("time-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, pp_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__UINT_POINTER,
                  G_TYPE_NONE, 2,
                  G_TYPE_INT,
                  G_TYPE_INT);

  xfmpc_mpdclient_signals[SIG_VOLUME_CHANGED] =
    g_signal_new ("volume-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, pp_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  xfmpc_mpdclient_signals[SIG_STOPPED] =
    g_signal_new ("stopped", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, pp_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

static void
xfmpc_mpdclient_init (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  xfmpc_mpdclient_initenv (mpdclient);
  priv->mi = mpd_new (priv->host, priv->port, priv->passwd);
  mpd_signal_connect_status_changed (priv->mi, (StatusChangedCallback)cb_xfmpc_mpdclient_status_changed, mpdclient);
}

static void
xfmpc_mpdclient_finalize (GObject *object)
{
  XfmpcMpdclient *mpdclient = XFMPC_MPDCLIENT (object);
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_free (priv->mi);
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



XfmpcMpdclient *
xfmpc_mpdclient_new ()
{
  static XfmpcMpdclient *mpdclient = NULL;

  if (G_UNLIKELY (NULL == mpdclient))
    {
      mpdclient = g_object_new (XFMPC_TYPE_MPDCLIENT, NULL);
      g_object_add_weak_pointer (G_OBJECT (mpdclient), (gpointer)&mpdclient);
    }
  else
    g_object_ref (G_OBJECT (mpdclient));

  return mpdclient;
}

static void
xfmpc_mpdclient_initenv (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  /* Hostname */
  priv->host = (g_getenv ("MPD_HOST") != NULL) ?
      g_strdup (g_getenv ("MPD_HOST")) :
      g_strdup (MPD_HOST);

  /* Port */
  priv->port = (g_getenv ("MPD_PORT") != NULL) ?
      (gint) g_ascii_strtoll (g_getenv ("MPD_PORT"), NULL, 0) :
      MPD_PORT;

  /* Check for password */
  priv->passwd = NULL;
  gchar **split = g_strsplit (priv->host, "@", 2);
  if (g_strv_length (split) == 2)
    {
      g_free (priv->host);
      priv->host = g_strdup (split[0]);
      priv->passwd = g_strdup (split[1]);
    }
  g_strfreev (split);
}

gboolean
xfmpc_mpdclient_connect (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (xfmpc_mpdclient_is_connected (mpdclient))
    return TRUE;

  if (mpd_connect (priv->mi) != MPD_OK)
    return FALSE;

  mpd_send_password (priv->mi);

  return TRUE;
}

void
xfmpc_mpdclient_disconnect (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (xfmpc_mpdclient_is_connected (mpdclient))
    mpd_disconnect (priv->mi);
}

gboolean
xfmpc_mpdclient_is_connected (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  return mpd_check_connected (priv->mi);
}

gboolean
xfmpc_mpdclient_previous (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_prev (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pp (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_playing (mpdclient))
    return xfmpc_mpdclient_pause (mpdclient);
  else
    return xfmpc_mpdclient_play (mpdclient);
}

gboolean
xfmpc_mpdclient_play (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_play (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pause (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_pause (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_stop (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_stop (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_next (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_next (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_volume (XfmpcMpdclient *mpdclient,
                            guint8 volume)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_status_set_volume (priv->mi, volume) < 0)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_song_time (XfmpcMpdclient *mpdclient,
                               guint time)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  if (mpd_player_seek (priv->mi, time) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

const gchar *
xfmpc_mpdclient_get_artist (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->artist))
    song->artist = g_strdup (_("n/a"));

  return song->artist;
}

const gchar *
xfmpc_mpdclient_get_title (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->title))
    song->title = g_path_get_basename (song->file);

  return song->title;
}

const gchar *
xfmpc_mpdclient_get_album (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->album))
    song->album = g_strdup (_("n/a"));

  return song->album;
}

const gchar *
xfmpc_mpdclient_get_date (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->date))
    song->date = g_strdup (_("n/a"));

  return song->date;
}

gint
xfmpc_mpdclient_get_time (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  return mpd_status_get_elapsed_song_time (priv->mi);
}

gint
xfmpc_mpdclient_get_total_time (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  return mpd_status_get_total_song_time (priv->mi);
}

guint8
xfmpc_mpdclient_get_volume (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  gint volume = mpd_status_get_volume (priv->mi);
  return (volume < 0) ? 100 : volume;
}

gboolean
xfmpc_mpdclient_is_playing (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  return mpd_player_get_state (priv->mi) == MPD_PLAYER_PLAY;
}

gboolean
xfmpc_mpdclient_is_stopped (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  return mpd_player_get_state (priv->mi) == MPD_PLAYER_STOP;
}

void
xfmpc_mpdclient_update_status (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT_GET_PRIVATE (mpdclient);

  mpd_status_update (priv->mi);
}

static void
cb_xfmpc_mpdclient_status_changed (MpdObj *mi,
                                   ChangedStatusType what,
                                   gpointer user_data)
{
  XfmpcMpdclient *mpdclient = XFMPC_MPDCLIENT (user_data);
  g_return_if_fail (G_LIKELY (NULL != user_data));

  if (what & MPD_CST_STATE)
    {
      gint state = mpd_player_get_state (mi);
      if (state == MPD_PLAYER_STOP)
        g_signal_emit_by_name (mpdclient, "stopped");
      else if (state == MPD_PLAYER_PLAY || state == MPD_PLAYER_PAUSE)
        g_signal_emit_by_name (mpdclient, "pp-changed",
                               state == MPD_PLAYER_PLAY);
    }

  if (what & MPD_CST_SONGID)
    g_signal_emit_by_name (mpdclient, "song-changed");

  if (what & MPD_CST_VOLUME)
    g_signal_emit_by_name (mpdclient, "volume-changed",
                           xfmpc_mpdclient_get_volume (mpdclient));

  if (what & (MPD_CST_ELAPSED_TIME|MPD_CST_TOTAL_TIME))
    g_signal_emit_by_name (mpdclient, "time-changed",
                           xfmpc_mpdclient_get_time (mpdclient),
                           xfmpc_mpdclient_get_total_time (mpdclient));
}

