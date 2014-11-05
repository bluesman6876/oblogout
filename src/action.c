/***************************************************************************
 *            action.c
 *
 *  Sat Jul 18 15:29:19 2009
 *  Copyright  2009  w00zy <w00zy@yandex.ru>
 *
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
	 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

/*
 *  These functions with the prefix "xfsm_" are taken from
 *  xfsm-shutdown-helper.c of xfce4-session with some modification.
 *  Copyright (c) 2003-2006 Benedikt Meurer <benny@xfce.org>
	 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __ACTION_H__
#include "action.h"
#endif

/* 

 */
LogoutAction logout_action = LOGOUT_ACTION_BREAK | LOGOUT_ACTION_EXIT;

static gboolean sb_method_dbus (const char *destination, 
                                const char *path, 
                                const char *interface, 
                                const char *method ) ;
/* The spec:
	 http://upower.freedesktop.org/docs/UPower.html
	 http://www.freedesktop.org/software/ConsoleKit/doc/ConsoleKit.html
	 */

void sb_available_actions()
{
	/* check if method is available */
	int ConsoleKit = sb_method_dbus (CK_DBUS_DEST,
	                                 CK_DBUS_PATH,
	                                 CK_DBUS_FACE,
	                                 "GetSessions");

	int UPower = sb_method_dbus (U_DBUS_DEST,
	                             U_DBUS_PATH,
	                             U_DBUS_FACE,
	                             "EnumerateDevices");
	/* set method */
	if( G_LIKELY(ConsoleKit && UPower) )
		logout_action = LOGOUT_ACTION_SHUTDOWN |
		LOGOUT_ACTION_REBOOT |
		LOGOUT_ACTION_SUSPEND |
		LOGOUT_ACTION_HIBERNATE |
		LOGOUT_ACTION_BREAK |
		LOGOUT_ACTION_EXIT;
	else if ( ConsoleKit )
		logout_action = LOGOUT_ACTION_SHUTDOWN |
		LOGOUT_ACTION_REBOOT |
		LOGOUT_ACTION_BREAK |
		LOGOUT_ACTION_EXIT;
	else
		g_error(G_STRLOC ": Failed: ConsoleKit and  UPower not aviable!");
}

static gboolean 
sb_method_dbus (const char *destination, 
                const char *path, 
                const char *interface, 
                const char *method) 
{

	DBusConnection *connection;
	DBusMessage        *message;
	DBusMessage        *result;
	DBusError             error;

	/* initialize the error */
	dbus_error_init (&error);

	/* connect to the system message bus */
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, &error);

	if (G_UNLIKELY (connection == NULL)) {
		g_warning (G_STRLOC ": Failed to connect to DBUS: %s", error.message);
		dbus_error_free (&error);
		return FALSE;
	}

	message = dbus_message_new_method_call (destination, 
	                                        path, 
	                                        interface,
	                                        method);
	if (G_UNLIKELY (message == NULL)) {
		g_warning (G_STRLOC ": Failed to contact %s: %s", 
		           interface, 
		           error.message);
		return FALSE;
	}

	result = dbus_connection_send_with_reply_and_block (connection, message, 2000, &error);
	dbus_message_unref (message);

	/* translate error results appropriately */
	if (G_UNLIKELY(result == NULL || dbus_set_error_from_message (&error, result))) {
		g_debug("%p %s", result, error.message);
		g_debug("1");
		/* release and reset the result */
		dbus_message_unref (result);
		result = NULL;
	}
	else if (G_LIKELY(result != NULL)) {
		/* we received a valid message return?! */
		dbus_message_unref (result);
		dbus_error_free (&error);
		return TRUE;
	} else {
	}

	/* otherwise, we failed for some reason */
	g_warning (G_STRLOC ": %s error: not present?", destination);
	dbus_error_free (&error);
	return FALSE;
}

void
sb_button_callback (GtkWidget *widget, 
                    GdkEventButton *event, 
                    LogoutAction data) 
{

	switch(data)
	{
		case LOGOUT_ACTION_BREAK:
		{
			sb_destroy(widget, event, NULL);
			break;
		}
		case LOGOUT_ACTION_EXIT:
		{
			g_spawn_command_line_async( WM_METHOD_EXIT, NULL );
			sb_destroy(widget, event, NULL);
			break;			
		}
		case LOGOUT_ACTION_REBOOT:
		{
			if ( sb_method_dbus (CK_DBUS_DEST,
			                     CK_DBUS_PATH,
			                     CK_DBUS_FACE,
			                     "Restart"))
				g_spawn_command_line_async( WM_METHOD_EXIT, NULL );
			sb_destroy(widget, event, NULL);
			break;
		}
		case LOGOUT_ACTION_SHUTDOWN:
		{
			if ( sb_method_dbus (CK_DBUS_DEST,
			                     CK_DBUS_PATH,
			                     CK_DBUS_FACE,
			                     "Stop") )
				g_spawn_command_line_async( WM_METHOD_EXIT, NULL );
			sb_destroy(widget, event, NULL);
			break;			
		}
		case LOGOUT_ACTION_SUSPEND:
		{
			sb_method_dbus (U_DBUS_DEST,
			                U_DBUS_PATH,
			                U_DBUS_FACE,
			                "Suspend");
			sb_destroy(widget, event, NULL);
			break;
		}
		case LOGOUT_ACTION_HIBERNATE:
		{
			sb_method_dbus (U_DBUS_DEST,
			                U_DBUS_PATH,
			                U_DBUS_FACE,
			                "Hibernate");
			sb_destroy(widget, event, NULL);
			break;			
		}
		default:
			break;
	}

}

void 
sb_normal_button_state (GtkWidget *widget, 
                        GdkEventButton *event, 
                        GtkWidget *data)
{
	gtk_widget_set_state(widget, GTK_STATE_NORMAL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_hash_table_lookup(nHash, GTK_WIDGET(data) ));
}

void 
sb_enter_button_state (GtkWidget *widget, 
                       GdkEventButton *event, 
                       GtkWidget *data)
{
	gtk_widget_set_state(widget, GTK_STATE_PRELIGHT);
	gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_hash_table_lookup(pHash, GTK_WIDGET(data) ));
}

void
sb_destroy (GtkWidget *widget, 
            GdkEventButton *event, 
            gpointer user_data) 
{
	gtk_main_quit ();
	return;
}
