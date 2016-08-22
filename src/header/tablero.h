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


#ifndef _TABLERO_H
#define _TABLERO_H

// número de vecinos por cada célula (direcciones: vertical, horizontal, diagonal)
#define CELULA_N_VECINOS 							1

#define CELVIVA 									'*'
#define CELMUERTA 									'.'

#define ELEMENTOS 									sizeof(reglas) - 1
#define STEPS(x) (x < CELULA_N_VECINOS)

void crear_tablero_referencia_universo(int n);
void destruir_tablero_referencia_universo(int n);
void agregar_tablero_referencia_celula(int x, int y, int tipo);

void crear_vector_referencia_celula_vida_muerte(int n_vida, int n_muerte);
void destruir_vector_referencia_celula_vida_muerte(void);
void reproducir_universo(int n);

#endif
