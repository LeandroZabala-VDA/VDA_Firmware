/*
 * filters.h
 *
 *  Created on: Mar 20, 2025
 *      Author: leank
 */

#ifndef INC_FILTERS_H_
#define INC_FILTERS_H_


#include <stdint.h>

#define WINDOW_SIZE 30  // Tamaño de la ventana para el filtro de media movil

#define     FIR_ORDER
#define 	FILTER_ORDER 2  // Orden del filtro Notch

static const double num_notch[] = {0.969531, 0.336715, 0.969531};
static const double den_notch[] = {1.0000, 0.336715, 0.939063};

// donde se guardan los coeficientes
extern double x_notch[FILTER_ORDER + 1];
extern double y_notch[FILTER_ORDER + 1];


uint32_t filtroMediaMovil(uint16_t nuevaMuestra);
double iir_filter(double input, const double *num, const double *den, int num_order, int den_order, double *x, double *y);
double fir_filter(int16_t input, const double *coef,int order,double *x);

#endif /* INC_FILTERS_H_ */
