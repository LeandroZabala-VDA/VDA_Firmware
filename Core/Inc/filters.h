/*
 * filters.h
 *
 *  Created on: Mar 20, 2025
 *      Author: leank
 */

#ifndef INC_FILTERS_H_
#define INC_FILTERS_H_

#include <stdint.h>

//------------------ FILTRO MEDIA MOVIL -------------------
//------------------ FILTRO MEDIA MOVIL -------------------

#define WINDOW_SIZE_MAX 30 // Tamaño máximo de ventana que soportaremos

typedef struct {
    uint16_t ventana[WINDOW_SIZE_MAX];
    uint8_t indice;
    uint32_t sumaVentana;
    uint8_t tamanoVentana;  // Nuevo campo para tamaño de ventana variable
} FiltroMediaMovil;

void FiltroMediaMovil_Init(FiltroMediaMovil *filtro, uint8_t tamanoVentana);
uint32_t FiltroMediaMovil_Update(FiltroMediaMovil *filtro, uint16_t nuevaMuestra);


//------------------ FILTRO IIR -------------------

#define     FIR_ORDER
#define 	FILTER_ORDER 4  // Orden del filtro Notch

static const double num_notch[] = {0.905976651535332,   0.630821427483968,   1.921761782323010,   0.630821427483968,   0.905976651535332};
static const double den_notch[] = {1.000000000000000,   0.662012848994301,   1.912901767299951,   0.599630005973637,   0.820813318093725};


// donde se guardan los coeficientes
extern double x_notch[FILTER_ORDER + 1];
extern double y_notch[FILTER_ORDER + 1];

uint16_t iir_filter(uint16_t input, const double *num, const double *den, int num_order, int den_order, double *x, double *y);
double fir_filter(int16_t input, const double *coef,int order,double *x);

#endif /* INC_FILTERS_H_ */
