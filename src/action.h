/***************************************************************************
 *            action.h
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
 *
 */
#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifndef CFG_H
#include "setting.h"
#endif

#ifndef __ACTION_H__
#define __ACTION_H__

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <glib/gprintf.h>

#define U_DBUS_DEST "org.freedesktop.UPower"
#define U_DBUS_PATH "/org/freedesktop/UPower"
#define U_DBUS_FACE "org.freedesktop.UPower"

#define CK_DBUS_DEST "org.freedesktop.ConsoleKit"
#define CK_DBUS_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CK_DBUS_FACE "org.freedesktop.ConsoleKit.Manager"

#define WM_METHOD_EXIT "openbox --exit"


typedef enum {
        LOGOUT_ACTION_NONE     = 0,
        LOGOUT_ACTION_SHUTDOWN = 1 << 0,
        LOGOUT_ACTION_REBOOT   = 1 << 1,
        LOGOUT_ACTION_SUSPEND  = 1 << 2,
		LOGOUT_ACTION_HIBERNATE = 1 << 3,
        LOGOUT_ACTION_BREAK  = 1 << 8,
		LOGOUT_ACTION_EXIT = 1 << 9
} LogoutAction;

extern LogoutAction logout_action;

extern GHashTable *nHash;
extern GHashTable *pHash;

void sb_available_actions();

void sb_destroy (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void sb_button_callback (GtkWidget *widget, GdkEventButton *event, LogoutAction data);

void sb_normal_button_state (GtkWidget *widget, GdkEventButton *event, GtkWidget *data);
void sb_enter_button_state(GtkWidget *widget, GdkEventButton *event, GtkWidget *data);

#endif
