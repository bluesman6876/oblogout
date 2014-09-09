/***************************************************************************
 *            setting.h
 *
 *  Sun Jul 19 15:16:05 2009
 *  Copyright  2009  w00zy
 *  <w00zy@<host>>
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
#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifndef CFG_H
#define CFG_H

#include <gtk/gtk.h>

typedef struct _setting {
	gchar *theme;
	guint8 buttonsize;
	gchar *labelcolor;
	gchar *label_font;
	GdkColor color_label;
} Config;

extern Config cfg;

void sb_config_load(void);

#endif
