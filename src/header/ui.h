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


#ifndef _UI_H
#define _UI_H

#define CELULA_TOP_PIXELS							0
#define CELULA_BUTTOM_PIXELS						0
#define CELULA_RIGHT_PIXELS							10
#define CELULA_LEFT_PIXELS							10
#define CELULA_BORDER_PIXELS						1

#define GLADE_FILE									"el_juego_de_la_vida.glade"
#define CSS_FILE									"ui.css"

#define GTK_BUILD_N_OBJECTS 						28

#define WINDOW_PRINCIPAL 							widgetList[0]
#define BARMENU_ITEM_N_UNIVERSO						widgetList[1]
#define BARMENU_ITEM_C_UNIVERSO						widgetList[2]
#define BARMENU_ITEM_G_UNIVERSO						widgetList[3]
#define BARMENU_ITEM_SALIR							widgetList[4]
#define BARMENU_ITEM_ACERCADE						widgetList[5]
#define BOTON_LOAD_POS								widgetList[6]
#define BOTON_PARAR									widgetList[7]
#define BOTON_PLAY_PAUSE							widgetList[8]
#define BOTON_AVANZAR								widgetList[9]
#define GRID_UNIVERSO								widgetList[10]
#define LABEL_ITERACION								widgetList[11]

#define WINDOW_UNIVERSO								widgetList[12]
#define SPINBUTTON_FILA_COLUMNA						widgetList[13]
#define SPINBUTTON_CELULA_VIVA						widgetList[14]
#define SPINBUTTON_CELULA_MUERTA					widgetList[15]
#define SPINBUTTON_ITERACION						widgetList[16]
#define BOTON_ACEPTAR								widgetList[17]
#define BOTON_CANCELAR								widgetList[18]

#define WINDOW_ABOUT_DIALOG							widgetList[19]
#define WINDOW_MESSAGE_DIALOG						widgetList[20]
#define WINDOW_FILECHOOSER_CARPETA					widgetList[21]
#define WINDOW_FILECHOOSER_ARCHIVO					widgetList[22]
#define ADJUSTMENT_FILA_COLUMNA						widgetList[23]

#define DIALOG_MSG_ACEPTAR							widgetList[24]
#define DIALOG_MSG_CANCELAR							widgetList[25]
#define STATUSBAR_ESTADO							widgetList[26]
#define BOTON_SAVE_POS								widgetList[27]

void administrar_widgets_builder(void);
void widgets_asignar_propiedades(void);
void widgets_conectar_signals(void);
void ui_mostar_informe(char *str);
void print_cel_values(char **universo, int n);
void tablero_asignar_celula_tipo(int tx, int ty, int tipo);
void statusbar_iteraciones_change_text(int vida, int muerte, int count, int niteraciones);

void obtener_max_fc_dibujables_en_pantalla();

#define STATUSBAR_ITERACIONES_TEXT 				"Cantidad de veces que se aplico la regla"

#endif
