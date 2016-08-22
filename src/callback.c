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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "header/ui.h"
#include "header/callback.h"
#include "header/tablero.h"

// GCC
// enumeration value '...' not handled in switch [-Wswitch]
#pragma GCC diagnostic ignored "-Wswitch"

/* ==============================================================================================
 	TIPOS DE DATOS
   ============================================================================================= */

enum
{
	CELULA_MUERTA,
	CELULA_VIVA
};

typedef enum
{
	JUEGO_TABLERO_SIN_DEFINIR,
	JUEGO_TABLERO_DEFINIDO
} Tablero;

typedef enum
{
	JUEGO_INFORME_NINGUNO,
	JUEGO_INFORME_NUEVO_TABLERO,
} JuegoInforme;

typedef enum
{
	JUEGO_ACCION_NINGUNA,
	JUEGO_ACCION_PLAY,
	JUEGO_ACCION_PAUSE,	// el universo: pausa
	JUEGO_ACCION_STOP,	// el universo: estado inicial
	JUEGO_ACCION_NEXT,
	JUEGO_ACCION_BACK
} JuegoAccion;

/* ==============================================================================================
 	 VARIABLES GLOBALES
   ============================================================================================= */

Tablero tablero_status		= JUEGO_TABLERO_SIN_DEFINIR;
JuegoAccion game_action 	= JUEGO_ACCION_NINGUNA;
JuegoInforme game_informe 	= JUEGO_INFORME_NINGUNO;

/* ==============================================================================================
 	 VARIABLES GLOBALES DEL DOCUMENTO
   ============================================================================================= */

static int n_filas_columnas_tablero;
static int iteraciones;
static int n_vida;
static int n_muerte;

static GtkWidget ***event_box_tablero_celulas;

static const int velocity_ms[N_VELOCITY] =
{
		2000,	// 2s
		1000, 	// 1s
		500, 	// 1/2 s
		250, 	// 1/4 s
		125,	// 1/8
		50
};

static int velocity_avanzar;
static int game_timeout_event_source;

/* ==============================================================================================
 	 VARIABLES EXTERNAS
   ============================================================================================= */

// main.c
extern void 		*widgetList[GTK_BUILD_N_OBJECTS];

// ui.c
extern GtkWidget ***tablero_celulas;

/* ==============================================================================================
 	 PROCEDIMIENTOS LOCALES DEL ARCHIVO
   ============================================================================================= */

static void crear_n_celulas_labels(void);
static void crear_universo(void);
static void juego_accion_borrar_tablero_existente(void);
static void celula_press_callback(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean game_timeout(gpointer user_data);
static void set_velocity_avanzar(void);
static void new_game_timeout(void);
static void remove_game_timeout(void);

/* ==============================================================================================
 	 FUNCIONES LOCALES DEL ARCHIVO
   ============================================================================================= */

static int get_velocity_avanzar(void);

/* ==============================================================================================
 	 PROCEDIMIENTOS EXTERNOS
   ============================================================================================= */

void barmenu_item_n_universo_callback(GtkMenuItem *menuitem, gpointer user_data)
{
	if (tablero_status == JUEGO_TABLERO_SIN_DEFINIR)
	{
		gtk_widget_show_all(GTK_WIDGET(WINDOW_UNIVERSO));
	}
	else
	{
		game_informe = JUEGO_INFORME_NUEVO_TABLERO;
		ui_mostar_informe("Los datos del universo\nse perderan ¿estas seguro?");
	}
}

void barmenu_item_acercade_callback(GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_dialog_run(GTK_DIALOG(WINDOW_ABOUT_DIALOG));
	gtk_widget_hide(GTK_WIDGET(WINDOW_ABOUT_DIALOG));
}

void boton_aceptar_window_universo_callback(GtkButton *button, gpointer user_data)
{
	crear_universo();

	gtk_widget_hide(GTK_WIDGET(WINDOW_UNIVERSO));
	gtk_widget_show(GTK_WIDGET(STATUSBAR_ESTADO));
}

void boton_cancelar_window_universo_callback(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(WINDOW_UNIVERSO));
}

void window_universo_destroy_callback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_widget_hide_on_delete(GTK_WIDGET(WINDOW_UNIVERSO));
}

void dialog_msg_cancelar_callback(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(WINDOW_MESSAGE_DIALOG));
}

void dialog_msg_aceptar_callback(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(WINDOW_MESSAGE_DIALOG));

	switch (game_informe)
	{
		case JUEGO_INFORME_NUEVO_TABLERO:
			juego_accion_borrar_tablero_existente();
			gtk_widget_hide(GTK_WIDGET(STATUSBAR_ESTADO));
			gtk_widget_show_all(GTK_WIDGET(WINDOW_UNIVERSO));
			break;
	}
}

void boton_play_pause_window_universo_callback(GtkButton *button, gpointer user_data)
{
	if (tablero_status == JUEGO_TABLERO_SIN_DEFINIR)
		return;

	if (game_action == JUEGO_ACCION_PLAY) // parar celulas
	{
		gtk_button_set_label(button, "gtk-go-forward");
		game_action			 	= JUEGO_ACCION_PAUSE;
		velocity_avanzar 		= 0;
	}
	else // mover celulas
	{
		gtk_button_set_label(button, "gtk-media-pause");
		game_action = JUEGO_ACCION_PLAY;
	}

	new_game_timeout();
}

void boton_parar_window_universo_callback(GtkButton *button, gpointer user_data)
{
	if (tablero_status == JUEGO_TABLERO_SIN_DEFINIR)
		return;

	int i, j;
	game_action = JUEGO_ACCION_STOP;

	for (i = 0; i < n_filas_columnas_tablero; i++)
		for (j = 0; j < n_filas_columnas_tablero; j++)
			tablero_asignar_celula_tipo(i, j, CELULA_MUERTA);

	// inicializar contadores iterativos a cero
	game_timeout(NULL);
	remove_game_timeout();
	statusbar_iteraciones_change_text(n_vida, n_muerte, 0, iteraciones);
	velocity_avanzar = 0;

	// restablecer icono del botón play
	gtk_button_set_label(GTK_BUTTON(BOTON_PLAY_PAUSE), "gtk-go-forward");
}

void boton_avanzar_window_universo_callback(GtkButton *button, gpointer user_data)
{
	if (tablero_status == JUEGO_TABLERO_SIN_DEFINIR)
		return;

	game_action = JUEGO_ACCION_PLAY;
	gtk_button_set_label(GTK_BUTTON(BOTON_PLAY_PAUSE), "gtk-media-pause");

	set_velocity_avanzar();
	new_game_timeout();
}

void borrar_n_celulas(void)
{
	int i;

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		if (tablero_celulas[i] == NULL)
			continue;

		free(tablero_celulas[i]);
		tablero_celulas[i] = NULL;
	}

	if (tablero_celulas != NULL)
		free(tablero_celulas);

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		if (event_box_tablero_celulas[i] == NULL)
			continue;

		free(event_box_tablero_celulas[i]);
		event_box_tablero_celulas[i] = NULL;
	}

	if (event_box_tablero_celulas != NULL)
		free(event_box_tablero_celulas);

	destruir_tablero_referencia_universo(n_filas_columnas_tablero);
	destruir_vector_referencia_celula_vida_muerte();

	tablero_celulas				 	= NULL;
	event_box_tablero_celulas		= NULL;
	n_filas_columnas_tablero		= 0;

	remove_game_timeout();
}

/* ==============================================================================================
 	 PROCEDIMIENTOS LOCALES DEL ARCHIVO
   ============================================================================================= */

static void crear_n_celulas_labels(void)
{
	int i, j;

	// labels
	tablero_celulas = (GtkWidget ***) malloc( sizeof(GtkWidget **) * n_filas_columnas_tablero  );
	assert(tablero_celulas);

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		tablero_celulas[i] = (GtkWidget **) malloc( sizeof(GtkWidget *) * n_filas_columnas_tablero  );
		assert(tablero_celulas[i]);
	}

	// event box
	event_box_tablero_celulas = (GtkWidget ***) malloc( sizeof(GtkWidget **) * n_filas_columnas_tablero  );

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		event_box_tablero_celulas[i] = (GtkWidget **) malloc( sizeof(GtkWidget *) * n_filas_columnas_tablero  );
		assert(event_box_tablero_celulas[i]);
	}

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		for (j = 0; j < n_filas_columnas_tablero; j++)
		{
			tablero_celulas[i][j]			 	= gtk_label_new("");
			assert(tablero_celulas[i]);

			event_box_tablero_celulas[i][j] 	= gtk_event_box_new();
			assert(event_box_tablero_celulas[i][j]);

			gtk_widget_set_name(tablero_celulas[i][j], "celula_muerta");
			gtk_container_add(GTK_CONTAINER(event_box_tablero_celulas[i][j]), tablero_celulas[i][j]);
			gtk_widget_set_events(GTK_WIDGET(event_box_tablero_celulas[i][j]), GDK_BUTTON_PRESS_MASK);

			g_object_set_data(G_OBJECT(tablero_celulas[i][j]), "tx", GINT_TO_POINTER(i));
			g_object_set_data(G_OBJECT(tablero_celulas[i][j]), "ty", GINT_TO_POINTER(j));
			g_object_set_data(G_OBJECT(tablero_celulas[i][j]), "tipo_celula", GINT_TO_POINTER(CELULA_MUERTA));

			g_signal_connect
			(
				G_OBJECT(event_box_tablero_celulas[i][j]), 
				"button_press_event",
				G_CALLBACK(celula_press_callback),
				tablero_celulas[i][j]
			);

		}
	}
}

static void crear_universo(void)
{
	int i, j;

	GtkWidget *widget;

	n_vida 						= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(SPINBUTTON_CELULA_VIVA));
	n_muerte 					= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(SPINBUTTON_CELULA_MUERTA));
	n_filas_columnas_tablero 	= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(SPINBUTTON_FILA_COLUMNA));
	iteraciones 				= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(SPINBUTTON_ITERACION));

	crear_n_celulas_labels();

	gtk_grid_insert_row(GTK_GRID(GRID_UNIVERSO), n_filas_columnas_tablero);
	gtk_grid_insert_row(GTK_GRID(GRID_UNIVERSO), n_filas_columnas_tablero);

	for (i = 0; i < n_filas_columnas_tablero; i++)
	{
		for (j = 0; j < n_filas_columnas_tablero; j++)
		{
			widget = GTK_WIDGET(event_box_tablero_celulas[i][j]);
			gtk_grid_attach(GTK_GRID(GRID_UNIVERSO), widget, j, i, 1, 1);
		}
	}

	tablero_status 	= JUEGO_TABLERO_DEFINIDO;
	game_action 	= JUEGO_ACCION_STOP;	// el universo inicialmente está detenido

	crear_tablero_referencia_universo(n_filas_columnas_tablero);
	crear_vector_referencia_celula_vida_muerte(n_vida, n_muerte);

	statusbar_iteraciones_change_text(n_vida, n_muerte, 0, iteraciones);
	gtk_widget_show_all(GTK_WIDGET(GRID_UNIVERSO));
}

static void juego_accion_borrar_tablero_existente(void)
{
	int i, j;

	for (i = 0; i < n_filas_columnas_tablero; i++)
		for (j = 0; j < n_filas_columnas_tablero; j++)
			gtk_widget_destroy(event_box_tablero_celulas[i][j]);

	borrar_n_celulas();

	iteraciones 	= 0;
	n_vida			= 0;
	n_muerte		= 0;

	tablero_status 	= JUEGO_TABLERO_SIN_DEFINIR;
	game_action 	= JUEGO_ACCION_STOP;

	// inicializar contadores iterativos a cero
	game_timeout(NULL);
	velocity_avanzar = 0;
	remove_game_timeout();

	// restablecer icono del botón play
	gtk_button_set_label(GTK_BUTTON(BOTON_PLAY_PAUSE), "gtk-go-forward");
}

static void celula_press_callback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	if (game_action != JUEGO_ACCION_PAUSE && game_action != JUEGO_ACCION_STOP)
	{
		game_informe = JUEGO_INFORME_NINGUNO;
		ui_mostar_informe("No puedes modificar\nel tablero");
		return;
	}

	int tx 			= GPOINTER_TO_INT(g_object_get_data(G_OBJECT(user_data), "tx"));
	int ty 		 	= GPOINTER_TO_INT(g_object_get_data(G_OBJECT(user_data), "ty"));
	int tipo_celula = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(user_data), "tipo_celula"));

	if (tipo_celula == CELULA_MUERTA)
		tablero_asignar_celula_tipo(tx, ty, CELULA_VIVA);
	else
		tablero_asignar_celula_tipo(tx, ty, CELULA_MUERTA);
}

static gboolean game_timeout(gpointer user_data)
{
	static int x;

	if (game_action == JUEGO_ACCION_PAUSE)
		return FALSE;

	if (game_action == JUEGO_ACCION_STOP)
	{
		x = 0;
		return FALSE;
	}

	if (x >= iteraciones)
	{
		remove_game_timeout();

		// restablecer icono del botón play
		gtk_button_set_label(GTK_BUTTON(BOTON_PLAY_PAUSE), "gtk-go-forward");

		game_action = JUEGO_ACCION_STOP;
		game_informe = JUEGO_INFORME_NINGUNO;
		ui_mostar_informe("La reproduccion del\nuniverso ha concluido");

		return FALSE;
	}

	reproducir_universo(n_filas_columnas_tablero);

	x++;
	statusbar_iteraciones_change_text(n_vida, n_muerte, x, iteraciones);

	return TRUE;
}

static void set_velocity_avanzar(void)
{
	velocity_avanzar++;

	if (velocity_avanzar > N_VELOCITY - 1)
		velocity_avanzar = 0;
}

static void remove_game_timeout(void)
{
	if (game_timeout_event_source != 0)
		g_source_remove(game_timeout_event_source);

	game_timeout_event_source = 0;
}

static void new_game_timeout(void)
{
	remove_game_timeout();

	game_timeout_event_source = g_timeout_add(get_velocity_avanzar(), game_timeout, NULL);
	assert(game_timeout_event_source);
}

/* ==============================================================================================
 	 FUNCIONES LOCALES DEL ARCHIVO
   ============================================================================================= */

static int get_velocity_avanzar(void)
{
	return velocity_ms[velocity_avanzar];
}
