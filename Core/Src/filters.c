/*
 * filters.c
 *
 *  Created on: Mar 20, 2025
 *      Author: leank
 */

#include "filters.h"

// Inicializa un filtro de media móvil con tamaño de ventana específico
void FiltroMediaMovil_Init(FiltroMediaMovil *filtro, uint8_t tamanoVentana) {
    // Verificar que el tamaño de ventana no exceda el máximo
    if (tamanoVentana > WINDOW_SIZE_MAX) {
        tamanoVentana = WINDOW_SIZE_MAX;
    }

    // No permitir un tamaño de ventana de 0
    if (tamanoVentana == 0) {
        tamanoVentana = 1;
    }

    for (int i = 0; i < WINDOW_SIZE_MAX; i++) {
        filtro->ventana[i] = 0;
    }

    filtro->indice = 0;
    filtro->sumaVentana = 0;
    filtro->tamanoVentana = tamanoVentana;
}

// Aplica el filtro de media móvil a una nueva muestra
uint32_t FiltroMediaMovil_Update(FiltroMediaMovil *filtro, uint16_t nuevaMuestra) {
    filtro->sumaVentana = filtro->sumaVentana - filtro->ventana[filtro->indice] + nuevaMuestra;
    filtro->ventana[filtro->indice] = nuevaMuestra;
    filtro->indice = (filtro->indice + 1) % filtro->tamanoVentana; // Usar el tamaño de ventana específico del filtro
    return (uint32_t)(filtro->sumaVentana / filtro->tamanoVentana);
}

// -------------------------------------------------------------------------------------

// parametros:
// input: muestra
// *num : coeficientes del numerador
// *den coeficientes de denominador
// orden del numerador
//orden del nenominador
//donde se guardan los x calculados
// donde se guardan los y calculados

// Definición de las variables
double x_notch[FILTER_ORDER + 1] = {0};
double y_notch[FILTER_ORDER + 1] = {0};


uint16_t iir_filter(uint16_t input, const double *num, const double *den,
                         int num_order, int den_order, double *x, double *y) {
    // Convertir entrada de uint16_t a double para cálculos internos
    double input_double = (double)input;

    // Desplazar valores anteriores de x
    for (uint8_t i = num_order; i > 0; i--) {
        x[i] = x[i-1];
    }
    x[0] = input_double;

    // Calcular salida usando coeficientes del numerador
    double output = 0.0;
    for (uint8_t i = 0; i <= num_order; i++) {
        output += num[i] * x[i];
    }

    // Aplicar coeficientes del denominador
    for (int i = 1; i <= den_order; i++) {
        if(den[i] != 0.0){
            output -= den[i] * y[i - 1];
        }
    }

    // Normalizar por den[0]
    output /= den[0];

    // Desplazar valores anteriores de y
    for (uint8_t i = den_order; i > 0; i--) {
        y[i] = y[i-1];
    }
    y[0] = output;

    // Convertir salida a uint16_t con saturación
    if (output < 0) {
        return 0;
    } else if (output > 65535) {
        return 65535;
    } else {
        return (uint16_t)output;
    }
}

double fir_filter(int16_t input, const double *coef,int order,double *x){

    for(uint8_t i= order-1 ; i>0 ; i--){
        x[i]=x[i-1];
    }
    x[0]=(double)input;
    double out = 0.0;
    for(uint8_t i=0; i< order;i++){
        out+=x[i]*coef[i];
    }
    return out;
}
