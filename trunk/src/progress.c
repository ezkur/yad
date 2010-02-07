
#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "yad.h"

static GtkWidget *progress_bar;

gint pulsate_timeout (gpointer data);

static gboolean
pulsate_progress_bar (gpointer user_data)
{
  gtk_progress_bar_pulse (GTK_PROGRESS_BAR (progress_bar));
  return TRUE;
}

static gboolean
handle_stdin (GIOChannel * channel, GIOCondition condition, gpointer data)
{
  static gint pulsate_timeout = -1;
  float percentage = 0.0;

  if ((condition == G_IO_IN) || (condition == G_IO_IN + G_IO_HUP))
    {
      GString *string;
      GError *err = NULL;

      string = g_string_new (NULL);

      if (options.progress_data.pulsate)
	{
	  if (pulsate_timeout == -1)
	    pulsate_timeout = g_timeout_add (100, pulsate_progress_bar, NULL);
	}

      while (channel->is_readable != TRUE)
	;
      do
	{
	  gint status;

	  do
	    {
	      status =
		g_io_channel_read_line_string (channel, string, NULL, &err);

	      while (gtk_events_pending ())
		gtk_main_iteration ();

	    }
	  while (status == G_IO_STATUS_AGAIN);

	  if (status != G_IO_STATUS_NORMAL)
	    {
	      if (err)
		{
		  g_printerr ("yad_progress_handle_stdin(): %s",
			      err->message);
		  g_error_free (err);
		  err = NULL;
		}
	      continue;
	    }

	  if (!g_ascii_strncasecmp (string->str, "#", 1))
	    {
	      gchar *match;

	      /* We have a comment, so let's try to change the label */
	      match = g_strstr_len (string->str, string->len, "#");
	      match++;
	      /* FIXME: g_strcompress() return newly allocated string. so there is a little memory leak */
	      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress_bar),
					 g_strcompress (g_strstrip (match)));
	    }
	  else
	    {
	      if (!g_ascii_isdigit (*(string->str)))
		continue;

	      /* Now try to convert the thing to a number */
	      percentage = atoi (string->str);
	      if (percentage >= 100)
		{
		  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR
						 (progress_bar), 1.0);
		  if (options.progress_data.autoclose)
		    gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);
		}
	      else
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR
					       (progress_bar),
					       percentage / 100.0);
	    }

	}
      while (g_io_channel_get_buffer_condition (channel) == G_IO_IN);
      g_string_free (string, TRUE);
    }

  if (condition != G_IO_IN)
    {
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), 1.0);

      if (options.progress_data.pulsate)
	{
	  g_source_remove (pulsate_timeout);
	  pulsate_timeout = -1;
	}

      if (options.progress_data.autoclose)
	gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);

      g_io_channel_shutdown (channel, TRUE, NULL);
      return FALSE;
    }
  return TRUE;
}

GtkWidget *
progress_create_widget (GtkWidget * dlg)
{
  GtkWidget *w;
  GIOChannel *channel;

  w = progress_bar = gtk_progress_bar_new ();

  if (options.progress_data.percentage > -1)
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar),
				   options.progress_data.percentage / 100.0);
  if (options.progress_data.progress_text)
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (w),
			       options.progress_data.progress_text);
  if (options.progress_data.rtl)
    gtk_progress_bar_set_orientation (GTK_PROGRESS_BAR (w),
				      GTK_PROGRESS_RIGHT_TO_LEFT);

  channel = g_io_channel_unix_new (0);
  g_io_channel_set_encoding (channel, NULL, NULL);
  g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
  g_io_add_watch (channel, G_IO_IN | G_IO_HUP, handle_stdin, dlg);

  return w;
}
