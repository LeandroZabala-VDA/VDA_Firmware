/*
 * filters.c
 *
 *  Created on: Mar 20, 2025
 *      Author: leank
 */

#include "filters.h"

// Definición de las variables
double x_notch[FILTER_ORDER + 1] = {0};
double y_notch[FILTER_ORDER + 1] = {0};


// Aplica el filtro de media móvil sin necesidad de inicialización previa
uint32_t filtroMediaMovil(uint16_t nuevaMuestra) {
    static uint16_t ventana[WINDOW_SIZE] = {0};  // Inicializa en cero
    static uint8_t indice = 0;
    static uint32_t sumaVentana = 0;

    // Restar la muestra más antigua y sumar la nueva
    sumaVentana = sumaVentana - ventana[indice] + nuevaMuestra;
    ventana[indice] = nuevaMuestra;

    // Mover el índice circular
    indice = (indice + 1) % WINDOW_SIZE;

    // Retornar el valor filtrado
    return (uint32_t)(sumaVentana / WINDOW_SIZE);
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
