/***************************************************************************
 *            setting.c
 *
 *  Sun Jul 19 15:16:05 2009
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef CFG_H
#include "setting.h"
#endif

/* Base and на всякий пожарный случай */
static void
sb_safe_config(void)
{
	if (!cfg.theme)
		cfg.theme = g_strdup("white");

	if (!cfg.buttonsize)
		cfg.buttonsize = (int) 70;
	
	if (!cfg.label_font)
		cfg.label_font = g_strdup("Monospace Bold 10");
	
	if (!cfg.labelcolor)
		cfg.labelcolor = g_strdup("#E5E5E5");
}
/* Save a configuration to user keyfile ~/.config/PACKAGE/logout */
static void
config_save(void)
{
	GKeyFile *file = g_key_file_new();
	gchar * dir = g_build_filename(g_get_user_config_dir(), "oblogout" , NULL);
	gchar *path = g_build_filename(dir, "config", NULL);

	g_key_file_load_from_file(file, path, G_KEY_FILE_KEEP_COMMENTS, NULL);

	g_key_file_set_string(file, "general", "theme", cfg.theme);
	
	g_key_file_set_integer(file, "button", "size", cfg.buttonsize);
	
	g_key_file_set_string(file, "label", "font", cfg.label_font);
	g_key_file_set_string(file, "label", "color", cfg.labelcolor);

	gchar *buffer = g_key_file_to_data(file, NULL, NULL);
	g_key_file_free(file);

	/* write it to the cfg file */
	GIOChannel *channel = g_io_channel_new_file(path, "w", NULL);
	g_io_channel_write_chars(channel, buffer, -1, NULL, NULL);
	g_io_channel_unref(channel);
	g_free(buffer);
	g_free(path);
	g_free(dir);
}

/* Load a configuration from a keyfile pointer */
static void
config_load_from_keyfile(GKeyFile *file)
{
	cfg.theme = g_key_file_get_string(file, "general", "theme", NULL);
	
	cfg.buttonsize = (unsigned int) g_key_file_get_integer(file, "button", "size", NULL);

	cfg.label_font = g_key_file_get_string(file, "label", "font", NULL);
	cfg.labelcolor = g_key_file_get_string(file, "label", "color", NULL);
}

static void 
sb_parse_theme(void)
{
	gchar *dir = g_build_filename(g_get_home_dir(), ".themes/oblogout" , cfg.theme, NULL);
	if (g_file_test(dir, G_FILE_TEST_IS_DIR))
		g_snprintf(dir, 1024, "%s/%s/%s", g_get_home_dir(), ".themes/oblogout" ,cfg.theme);
	else
		g_snprintf(dir, 1024,"%s/%s/%s", PACKAGE_DATA_DIR, "oblogout",cfg.theme); 

	if (cfg.theme) {
		g_free(cfg.theme);
	}

	cfg.theme = g_strdup(dir);
	//g_print(dir);
	g_free(dir);
}

void
sb_config_load(void)
{
	GKeyFile *file = g_key_file_new();
		
	gchar * dir = g_build_filename(g_get_user_config_dir(), "oblogout" , NULL);
	gchar *path = g_build_filename(dir, "config", NULL);
	if (!g_file_test(path, G_FILE_TEST_EXISTS))
	{
		g_mkdir_with_parents(dir, 0700);
		sb_safe_config();
		config_save();
	}
	g_free(dir);
	g_key_file_load_from_file(file, path, G_KEY_FILE_KEEP_COMMENTS, NULL);
	config_load_from_keyfile(file);
	g_key_file_free(file);
	g_free(path);
	
	sb_safe_config();
	gdk_color_parse (cfg.labelcolor, &cfg.color_label);
	g_free(cfg.labelcolor);
	sb_parse_theme();
}
