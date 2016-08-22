/*
	The MIT License (MIT)
	Copyright (c) 2016 Ravf

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"), 
	to deal in the Software without restriction, including without limitation the 
	rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
	sell copies of the Software, and to permit persons to whom the Software is 
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all 
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "header/ui.h"
#include "header/callback.h"
#include "header/tablero.h"

/* ==============================================================================================
 	 VARIABLES EXTERNAS
   ============================================================================================= */

// main.c
extern void 		*widgetList[GTK_BUILD_N_OBJECTS];

/* ==============================================================================================
 	 VARIABLES GLOBALES DEL ARCHIVO
   ============================================================================================= */

static const char 	WidgetID[GTK_BUILD_N_OBJECTS][32] =
{
		{ "window_principal" 			},
		{ "item_n_universo" 			},
		{ "item_c_universo" 			},//
		{ "item_g_universo" 			},//
		{ "item_salir" 					},
		{ "item_acerca_de" 				},
		{ "button_load_pos" 			},
		{ "button_parar" 				},
		{ "button_play_pause" 			},
		{ "button_avanzar" 				},
		{ "grid_universo" 				},
		{ "label_estado_iteracion"		},

		{ "window_u"					},
		{ "spinbutton_fc" 				},
		{ "spinbutton_cv" 				},
		{ "spinbutton_cm" 				},
		{ "sp_i" 						},
		{ "b_a" 						},
		{ "b_c" 						},

		{ "aboutdialog" 				},
		{ "messagedialog_informe" 		},

		{ "filechooserdialog_carpeta"	},
		{ "filechooserdialog_archivo"	},
		{ "adjustment_fc" },

		{ "dialog_c" 					},
		{ "dialog_a" 					},
		{ "statusbar_estado"			},
		{ "button_save_pos" }
};

/* ==============================================================================================
 	 VARIABLES GLOBALES
   ============================================================================================= */

static GtkBuilder 	*builder;
int max_fc_dibujables_screen;

GtkWidget ***tablero_celulas;

GtkCssProvider 	* css;
GdkDisplay		* display;
GdkScreen 		* screen;

/* ==============================================================================================
 	 PROCEDIMIENTOS LOCALES DEL ARCHIVO
   ============================================================================================= */

static void ui_css_load(void);

/* ==============================================================================================
 	 PROCEDIMIENTOS
   ============================================================================================= */

void widgets_asignar_propiedades(void)
{
	// CSS
	ui_css_load();

	gtk_window_set_transient_for(GTK_WINDOW(WINDOW_ABOUT_DIALOG), GTK_WINDOW(WINDOW_PRINCIPAL));
	gtk_window_set_transient_for(GTK_WINDOW(WINDOW_MESSAGE_DIALOG), GTK_WINDOW(WINDOW_PRINCIPAL));

	obtener_max_fc_dibujables_en_pantalla();
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(ADJUSTMENT_FILA_COLUMNA), (gdouble) max_fc_dibujables_screen);
}

void widgets_conectar_signals(void)
{
	// ventana: principal

	g_signal_connect
	(
			G_OBJECT(WINDOW_PRINCIPAL),
			"destroy",
			G_CALLBACK(gtk_main_quit),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BARMENU_ITEM_N_UNIVERSO),
			"activate",
			G_CALLBACK(barmenu_item_n_universo_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BARMENU_ITEM_ACERCADE),
			"activate",
			G_CALLBACK(barmenu_item_acercade_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BARMENU_ITEM_SALIR),
			"activate",
			G_CALLBACK(gtk_main_quit),
			NULL
	);

	// botones
	g_signal_connect
	(
			G_OBJECT(BOTON_PLAY_PAUSE),
			"clicked",
			G_CALLBACK(boton_play_pause_window_universo_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BOTON_PARAR),
			"clicked",
			G_CALLBACK(boton_parar_window_universo_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BOTON_AVANZAR),
			"clicked",
			G_CALLBACK(boton_avanzar_window_universo_callback),
			NULL
	);

	// ventana: universo

	g_signal_connect
	(
			G_OBJECT(WINDOW_UNIVERSO),
			"delete-event",
			G_CALLBACK(window_universo_destroy_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BOTON_ACEPTAR),
			"clicked",
			G_CALLBACK(boton_aceptar_window_universo_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(BOTON_CANCELAR),
			"clicked",
			G_CALLBACK(boton_cancelar_window_universo_callback),
			NULL
	);

	// dialog: informe

	g_signal_connect
	(
			G_OBJECT(DIALOG_MSG_CANCELAR),
			"clicked",
			G_CALLBACK(dialog_msg_cancelar_callback),
			NULL
	);

	g_signal_connect
	(
			G_OBJECT(DIALOG_MSG_ACEPTAR),
			"clicked",
			G_CALLBACK(dialog_msg_aceptar_callback),
			NULL
	);
}

void ui_mostar_informe(char *str)
{

	gtk_message_dialog_format_secondary_text
	(
			(GtkMessageDialog *)WINDOW_MESSAGE_DIALOG,
			str,
			NULL
	);

	gtk_dialog_run(GTK_DIALOG(WINDOW_MESSAGE_DIALOG));
	gtk_widget_hide(GTK_WIDGET(WINDOW_MESSAGE_DIALOG));
}

/*
 * Crea una lista con los objetos que contiene el
 * builder, almacena la dirección de cada objeto
 * cuyo identificador es igual a WidgetID, en el
 * vector de punteros widgetList
*/

void administrar_widgets_builder(void)
{
	int i;

	if (builder == NULL)
	{
		builder 	 	 	= gtk_builder_new_from_file(GLADE_FILE);
		assert(builder);

		for (i = 0; i < GTK_BUILD_N_OBJECTS; i++)
		{
			widgetList[i] 	= gtk_builder_get_object(builder, WidgetID[i]);
			assert(widgetList[i]);
		}

		assert(i == GTK_BUILD_N_OBJECTS);
	}
}

// mayor resolución de pantalla = más celdas en
// el tablero

void obtener_max_fc_dibujables_en_pantalla()
{
	int screen_surface_area_pixels;
	int celula_surface_area_pixels;
	int window_principal_area_pixels;

	int
		window_principal_alto_pixels,
		window_principal_ancho_pixels,
		screen_alto_pixels,
		screen_ancho_pixels;

	gtk_window_get_size
	(
			GTK_WINDOW(WINDOW_PRINCIPAL),
			&window_principal_ancho_pixels,
			&window_principal_alto_pixels
	);

	screen_ancho_pixels = gdk_screen_get_width(screen);
	screen_alto_pixels = gdk_screen_get_height(screen);

	screen_surface_area_pixels = screen_ancho_pixels * screen_alto_pixels;

	window_principal_area_pixels = window_principal_alto_pixels * window_principal_ancho_pixels;
	celula_surface_area_pixels = CELULA_TOP_PIXELS + CELULA_BUTTOM_PIXELS;
	celula_surface_area_pixels *= (CELULA_RIGHT_PIXELS + CELULA_LEFT_PIXELS);

	max_fc_dibujables_screen = screen_surface_area_pixels / (celula_surface_area_pixels + window_principal_area_pixels + 4*CELULA_BORDER_PIXELS);
}

void print_cel_values(char **universo, int n)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (universo[i][j] == CELVIVA)
				tablero_asignar_celula_tipo(i, j, 1);
			else
				tablero_asignar_celula_tipo(i, j, 0);
		}
	}

}

void tablero_asignar_celula_tipo(int tx, int ty, int tipo)
{
	agregar_tablero_referencia_celula(tx, ty, tipo);
	g_object_set_data(G_OBJECT(tablero_celulas[tx][ty]), "tipo_celula", GINT_TO_POINTER(tipo));

	if (tipo == 0)
		gtk_widget_set_name(GTK_WIDGET(tablero_celulas[tx][ty]), "celula_muerta");
	else
		gtk_widget_set_name(GTK_WIDGET(tablero_celulas[tx][ty]), "celula_viva");
}

void statusbar_iteraciones_change_text(int vida, int muerte, int count, int niteraciones)
{
	static char text[64];
	sprintf(text, "%s %d/%d: %d de %d", STATUSBAR_ITERACIONES_TEXT, vida, muerte, count, niteraciones);
	gtk_label_set_text(GTK_LABEL(LABEL_ITERACION), text);
}

/* ==============================================================================================
 	 VARIABLES GLOBALES DEL ARCHIVO
   ============================================================================================= */

static void ui_css_load(void)
{
	css = gtk_css_provider_new();

	if (!gtk_css_provider_load_from_path(css, CSS_FILE, NULL))
		exit(-1);

	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);

	// las propiedades visuales de los widgets
	// se muestran en pantalla
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
