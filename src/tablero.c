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
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "header/ui.h"
#include "header/tablero.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

/* ==============================================================================================
 	 PROCEDIMIENTOS LOCALES DEL ARCHIVO
   ============================================================================================= */

static int c_vecinos_derecha(int x, int y, int n);
static int c_vecinos_izquierda(int x, int y, int n);
static int c_vecinos_arriba(int x, int y, int n);
static int c_vecinos_abajo(int x, int y, int n);
static int c_vecinos_diagonal_tr(int x, int y, int n);
static int c_vecinos_diagonal_tl(int x, int y, int n);
static int c_vecinos_diagonal_br(int x, int y, int n);
static int c_vecinos_diagonal_bl(int x, int y, int n);

/* ==============================================================================================
 	 VARIABLES GLOBALES DEL DOCUMENTO
   ============================================================================================= */

static char **universo;

static int *regla_nac;
static int *regla_vida;

static size_t length_regla_vida;
static size_t length_regla_nac;

/* ==============================================================================================
 	 PROCEDIMIENTOS EXTERNOS
   ============================================================================================= */

void crear_tablero_referencia_universo(int n)
{
	int i, j;

	universo = (char **) malloc(sizeof(char *) * n );
	assert(universo);

	for (i = 0; i < n; i++)
	{
		universo[i] = (char *) malloc(sizeof(char) * n );
		assert(universo[i]);
	}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			universo[i][j] = CELMUERTA;
}

void destruir_tablero_referencia_universo(int n)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
		if (universo[i] == NULL)
			continue;

		free(universo[i]);
		universo[i] = NULL;
	}

	if (universo != NULL)
		free(universo);

	universo = NULL;
}

void agregar_tablero_referencia_celula(int x, int y, int tipo)
{
	if (tipo == 0)
		universo[x][y] = CELMUERTA;
	else
		universo[x][y] = CELVIVA;
}

void crear_vector_referencia_celula_vida_muerte(int n_vida, int n_muerte)
{
	// la cantidad de caracteres de strvida
	// y strmuerte están basados en el máximo
	// valor del widget spin

	int i;
	char strvida[6];
	char strmuerte[6];

	sprintf(strvida, "%d", n_vida);
	sprintf(strmuerte, "%d", n_muerte);

	length_regla_vida	= strlen(strvida);
	length_regla_nac 	= strlen(strmuerte);

	regla_vida = (int *) malloc(sizeof(int) * (int)length_regla_vida);
	assert(regla_vida);

	regla_nac = (int *) malloc(sizeof(int) * (int)length_regla_nac);
	assert(regla_nac);

	// tabla ASCII
	// convertir los valores de tipo carácter a valores numéricos

	for (i = 0; i < length_regla_vida; i++)
	{
		regla_vida[i] = (int) strvida[i];
		regla_vida[i] -= 48;
	}

	for (i = 0; i < length_regla_nac; i++)
	{
		regla_nac[i] = (int) strmuerte[i];
		regla_nac[i] -= 48;
	}
}

void destruir_vector_referencia_celula_vida_muerte(void)
{
	if (regla_vida != NULL)
		free(regla_vida);

	if (regla_nac != NULL)
		free(regla_nac);

	regla_vida 		= NULL;
	regla_nac	 	= NULL;
}

void reproducir_universo(int n)
{
	int i, j, k, flag, count;
	char new_universo[n][n];

	// copiar el universo
	for (i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			new_universo[i][j] = universo[i][j];

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			/*
				 obtener la cantidad de células vecinas vivas
				 en las direcciones:
					 - arriba
					 - abajo
					 - izquierda
					 - derecha
					 - diagonal superior: derecha, izquierda
					 - diagonal inferior: derecha, izquierda
				 con respecto a la posición de la célula actual
			*/

			count = c_vecinos_derecha(i, j, n);
			count += c_vecinos_izquierda(i, j, n);
			count += c_vecinos_arriba(i, j, n);
			count += c_vecinos_abajo(i, j, n);
			count += c_vecinos_diagonal_tr(i, j, n);
			count += c_vecinos_diagonal_tl(i, j, n);
			count += c_vecinos_diagonal_br(i, j, n);
			count += c_vecinos_diagonal_bl(i, j, n);

			// ¿que tipo de célula es?
			switch (universo[i][j])
			{
				case CELVIVA:

					 flag = 0;
					 for (k = 0; k < length_regla_vida; k++)
					 {
						 if (regla_vida[k] == count)
						 {
							 flag = 1;
							 break;
						 }
					 }

					 if (flag == 0)
						 new_universo[i][j] = CELMUERTA;

					break;

				case CELMUERTA:

					 for (k = 0; k < length_regla_nac; k++)
					 {
						 if (regla_nac[k] == count)
						 {
							 new_universo[i][j] = CELVIVA;
							 break;
						 }
					 }

					break;
			}
		}
	}

	// establecer los cambios al universo
	for (i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			universo[i][j] = new_universo[i][j];

	print_cel_values(universo, n);
}

/* ==============================================================================================
 	 PROCEDIMIENTOS LOCALES DEL ARCHIVO
   ============================================================================================= */

// función: obtener la cantidad de vecinos vivos hacia la
// derecha con respecto a la posición de la célula actual
static int c_vecinos_derecha(int x, int y, int n)
{
	int j, k = 0, count = 0;

	for (j = y + 1; j < n; j++, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[x][j] == CELVIVA)
		{
			count++;
		}
	}
	return count;
}

// función: obtener la cantidad de vecinos vivos hacia la
// izquierda con respecto a la posición de la célula actual
static int c_vecinos_izquierda(int x, int y, int n)
{
	int j, k = 0, count = 0;

	for (j = y - 1;  j >= 0; j--, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[x][j] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos hacia
// arriba con respecto a la posición de la célula actual
static int c_vecinos_arriba(int x, int y, int n)
{
	int i, k = 0, count = 0;

	for (i = x - 1; i >= 0; i--, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][y] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos hacia
// abajo con respecto a la posición de la célula actual
static int c_vecinos_abajo(int x, int y, int n)
{
	int i, k = 0, count = 0;

	for (i = x + 1; i < n; i++, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][y] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos en la
// diagonal superior derecha con respecto a la posición
// de la célula actual
static int c_vecinos_diagonal_tr(int x, int y, int n)
{
	int i, j, k = 0, count = 0;

	for (i = x - 1, j = y + 1; i >= 0 && j < n; i--, j++, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][j] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos en la
// diagonal superior izquierda con respecto a la posición
// de la célula actual
static int c_vecinos_diagonal_tl(int x, int y, int n)
{
	int i, j, k = 0, count = 0;

	for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][j] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos en la
// diagonal inferior derecha con respecto a la posición
// de la célula actual
static int c_vecinos_diagonal_br(int x, int y, int n)
{
	int i, j, k = 0, count = 0;

	for (i = x + 1, j = y + 1; i < n && j < n; i++, j++, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][j] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}

// función: obtener la cantidad de vecinos vivos en la
// diagonal inferior izquierda con respecto a la posición
// de la célula actual
static int c_vecinos_diagonal_bl(int x, int y, int n)
{
	int i, j, k = 0, count = 0;

	for (i = x + 1, j = y - 1; i < n && j >= 0; i++, j--, k++)
	{
		if (!STEPS(k))
			break;

		if (universo[i][j] == CELVIVA)
		{
			count++;
		}
	}

	return count;
}
