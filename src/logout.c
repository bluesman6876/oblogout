/***************************************************************************
 *            logout.c
 *
 *  Sat Jul 25 18:29:31 2009
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

#include <gtk/gtk.h>
#include <glib/gprintf.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

/* me */
#include "setting.h"
#include "action.h"
/* me */

Config cfg;
GtkWidget *window;

GdkPixbuf *bg_btn_pixbuf = NULL;

GHashTable *nHash;
GHashTable *pHash;

/* create icon + background pixbuf */
static GdkPixbuf *
sb_enter_icon(GdkPixbuf *scaled)
{
	GdkPixbuf *tmp_pixbuf = gdk_pixbuf_copy(bg_btn_pixbuf);
	gdk_pixbuf_composite (scaled, tmp_pixbuf, 0, 0, 
	    cfg.buttonsize, 
	    cfg.buttonsize,
	    0.0, 0.0, 1.0, 1.0, 
	    GDK_INTERP_NEAREST, 
	    255);
	g_object_unref (scaled);
	return tmp_pixbuf;
}

static GdkPixbuf *
sb_scaled_icon(const char* iconpath)
{
	GdkPixbuf *pixbuf = NULL;
	if (g_file_test(iconpath, G_FILE_TEST_EXISTS))
	{

		pixbuf = gdk_pixbuf_new_from_file_at_scale( iconpath,
		    cfg.buttonsize, 
		    cfg.buttonsize,
		    TRUE, NULL );
		return pixbuf;
	}

	GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
	pixbuf = gtk_icon_theme_load_icon (icon_theme,
	    "gtk-dialog-warning", /* имя значка */
	    cfg.buttonsize, /* размер */
	    0,  /* флаг */
	    NULL);
	return pixbuf;
}

/* prepare images hash */
static void
sb_init_images_data()
{
	gchar *bg_name = NULL;
	nHash = g_hash_table_new(g_direct_hash, NULL);
	pHash = g_hash_table_new(g_direct_hash, NULL);

	bg_name = g_strdup_printf("%s/%s", cfg.theme,"light.png");
	if (G_LIKELY(g_file_test (bg_name, G_FILE_TEST_EXISTS)))
	{
		bg_btn_pixbuf = gdk_pixbuf_new_from_file_at_scale(bg_name,
		    cfg.buttonsize, cfg.buttonsize, TRUE, NULL);
	}	else	{
		bg_btn_pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8,
		    cfg.buttonsize, cfg.buttonsize);
	}
	g_free(bg_name);
}

GdkPixbuf*
sb_init_images_hash(const char* img_name, GtkWidget* widget)
{
	gchar *full_name = NULL;
	GdkPixbuf *p = NULL;
	GdkPixbuf *n = NULL;
	
	full_name = g_strdup_printf("%s/%s", cfg.theme, img_name);

	n = sb_scaled_icon( full_name );
	g_hash_table_insert(nHash, widget, n);

	p = sb_enter_icon( gdk_pixbuf_copy( n ) );
	g_hash_table_insert( pHash, widget, p );

	g_free(full_name);
	return n;
}

/* prepage button */
/*-- This function responds to the mouse click on the button --*/
GtkWidget*
sb_create_button(const char* name, 
    const char* label_text, 
    const char* img_name, 
    LogoutAction response )
{
	GtkWidget *button,*vbox,*label;
	GtkWidget *img;
	/* create event box */
	button = gtk_event_box_new();
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(button),FALSE); /* !!!! */
	gtk_event_box_set_above_child(GTK_EVENT_BOX(button),TRUE);

	img = gtk_image_new();
	GdkPixbuf *p = sb_init_images_hash(img_name, GTK_WIDGET(img));

	gtk_image_set_from_pixbuf(GTK_IMAGE(img), p);

	/*-- Connect all the buttons to the button_was_clicked function --*/
	gtk_signal_connect(GTK_OBJECT(button),
	    "button-press-event", GTK_SIGNAL_FUNC(sb_button_callback),
	    GINT_TO_POINTER(response) );
	
	gtk_signal_connect(GTK_OBJECT(button),
	    "leave_notify_event", GTK_SIGNAL_FUNC(sb_normal_button_state),
	    GTK_WIDGET(img) );

	gtk_signal_connect(GTK_OBJECT(button),
	    "enter_notify_event", GTK_SIGNAL_FUNC(sb_enter_button_state),
	    GTK_WIDGET(img) );
	
	/* start pack */
	gtk_container_set_border_width(GTK_CONTAINER(button), 0);
	gtk_container_add(GTK_CONTAINER(button), img);

	/* Label with mnemonic ALT+KEY */
	label = gtk_label_new(FALSE);
	/* set text atribute */
	PangoFontDescription *desc = pango_font_description_from_string
	    (cfg.label_font);
	gtk_widget_modify_font (GTK_WIDGET(label), desc);
	pango_font_description_free(desc);
	gtk_widget_modify_fg (label, GTK_STATE_NORMAL, &cfg.color_label);

	gtk_label_set_markup_with_mnemonic(GTK_LABEL(label),label_text);
	/* set mnemonic button activate */
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), button);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(button), FALSE, FALSE, cfg.buttonsize / 10 );
	gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(label), FALSE, FALSE, 0 );

	return vbox;
}

GdkPixmap* 
sb_create_background (void)
{
	GdkPixbuf *shot;
	GdkScreen *screen;
	GdkPixmap *background;
	guchar *pixels, *p;
	int x, y, width, height, rowstride;

	gboolean has_alpha;
	/* make the background */
	screen = gdk_screen_get_default();

	shot = gdk_pixbuf_get_from_drawable( NULL,
	    gdk_get_default_root_window(),
	    gdk_rgb_get_colormap(),
	    0, 0, 0, 0,
	    gdk_screen_get_width(screen),
	    gdk_screen_get_height(screen));

	pixels = gdk_pixbuf_get_pixels(shot);
	width = gdk_pixbuf_get_width(shot);
	height = gdk_pixbuf_get_height(shot);
	has_alpha = gdk_pixbuf_get_has_alpha(shot);
	rowstride = gdk_pixbuf_get_rowstride(shot);
	for (y = 0; y < height; y++)
	{
		p = pixels;
		for (x = 0; x < width; x++)
		{
			p[0] = p[0] / 3;
			p[1] = p[1] / 3;
			p[2] = p[2] / 3;
			if( has_alpha )
				p += 4;
			else
				p += 3;
		}
		pixels += rowstride;
	}

	gdk_pixbuf_render_pixmap_and_mask_for_colormap(shot,
	    gdk_rgb_get_colormap(),
	    &background,
	    NULL,
	    255);

	g_object_unref (shot);
	return background;
}

void
sb_create_window(void)
{
	/*-- Declare the GTK Widgets used in the program --*/
	GtkWidget *button,*hbuttonbox;
	//GdkPixmap *background;
	GtkStyle *style;
	/* make background */
	style = gtk_style_new ();
	style->bg_pixmap[0] = sb_create_background();

	/*-- Create the new window --*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	/* destroy_event */
	gtk_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(sb_destroy), NULL);

	/* add style (background) nj window */
	gtk_widget_set_style (GTK_WIDGET(window), GTK_STYLE(style));
	g_object_unref(style);

	/* decorate and fullscreen */
	gtk_window_set_decorated( GTK_WINDOW(window), FALSE );
	gtk_window_fullscreen( GTK_WINDOW(window) );
	gtk_widget_realize (window);

	/*-- Create the hbox --*/
	sb_init_images_data();

	hbuttonbox = gtk_hbutton_box_new();
	gtk_box_set_spacing(GTK_BOX(hbuttonbox),cfg.buttonsize / 6);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox),GTK_BUTTONBOX_CENTER);
	gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox), 0);
	gtk_box_set_homogeneous(GTK_BOX(hbuttonbox), TRUE);

	/* Create button */	
	/*-- Add all the buttons to the vbox --*/
	button = sb_create_button("cancel", _("_Cancel"), "cancel.png", LOGOUT_ACTION_BREAK);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);

	button = sb_create_button("logout", _("E_xit"), "logout.png", LOGOUT_ACTION_EXIT);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);

	/* if hal exist? А нахуй тогда огород городить? */
	/* нехрена проверять, отдавать и всё. коли ставиш, знаеш зачем */
	sb_available_actions();
	
	if( G_LIKELY( logout_action & LOGOUT_ACTION_SUSPEND ) )
	{
		/* Create button */
		button = sb_create_button("logout", _("_Suspend"), "suspend.png", LOGOUT_ACTION_SUSPEND);
		/*-- Add all the buttons to the vbox --*/
		gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);	
	}

	if( G_LIKELY( logout_action & LOGOUT_ACTION_HIBERNATE ) )
	{
		button = sb_create_button("logout", _("_Hibernate"), "hibernate.png", LOGOUT_ACTION_HIBERNATE);
		gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);
	}

	if( G_LIKELY( logout_action & LOGOUT_ACTION_REBOOT ) )
	{
		button = sb_create_button("logout", _("_Reboot"), "restart.png", LOGOUT_ACTION_REBOOT);
		gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);
	}

	if( G_LIKELY( logout_action & LOGOUT_ACTION_SHUTDOWN ))
	{
		button = sb_create_button("logout", _("Sh_utdown"), "shutdown.png", LOGOUT_ACTION_SHUTDOWN);
		gtk_box_pack_start(GTK_BOX(hbuttonbox), button, FALSE, FALSE, 0);
	}

	/* clear images pixbuf */
	g_object_unref(bg_btn_pixbuf);

	/*-- Add the button to the window --*/
	gtk_container_add(GTK_CONTAINER (window), hbuttonbox);

	/*-- Add a border to the window to give the buttons a little room --*/
	gtk_container_border_width (GTK_CONTAINER (window), 20);

	gtk_widget_show_all(GTK_WIDGET(window) );
}

int main (int argc, char *argv[])
{
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	/*-- Load config state --*/
	sb_config_load();
	/*-- go main --*/
	sb_create_window();
	/*-- Start the GTK event loop --*/
	gtk_main();
	/*-- Return 0 if exit is successful --*/
	return 0;
}
