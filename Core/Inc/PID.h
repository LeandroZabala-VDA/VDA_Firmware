/*
 * PID.h
 *
 *  Created on: Aug 2, 2023
 *      Author: leank
 */

#ifndef SRC_PID_H_
#define SRC_PID_H_


#define PID_KP  0.02
#define PID_KI  0.01
#define PID_KD  0.0

// Limite maximo y minimo del PID
#define PID_LIM_MIN 0
#define PID_LIM_MAX  4095.0

// Periodo de muestreo en ms.
#define T_MUESTREO 5.555

// Limite maximo y minimo del termino integral
#define PID_LIM_MIN_INT -500.0
#define PID_LIM_MAX_INT  2500.0

typedef struct{

	// Ganancias del controlador

	float Kp;
	float Ki;
	float Kd;

	// Filtro pasa bajos para el termino derivativo
//	float tau;

	// Limites de la salida del controlador

	float limMin;
	float limMax;

	// Limites del integrador
	float limMinInt;
	float limMaxInt;

	// Periodo de muestreo

	float T;

	// Memoria del controlador

	float integrador;
	float errorprev;
	float derivador;
	float medicionPrev;

	// Salida del controlador

	float out;
}PIDController;

void PIDController_Init(PIDController *pid);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);

#endif /* SRC_PID_H_ */
