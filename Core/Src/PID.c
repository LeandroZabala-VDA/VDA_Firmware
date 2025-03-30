
/*
 * PID.c
 *
 *  Created on: Aug 2, 2023
 *      Author: leank
 */
#include "PID.h"


void PIDController_Init(PIDController *pid){
	// Hacemos cero las variables

	pid->integrador=0.0;
	pid->errorprev=0.0;
	pid->derivador=0.0;
	pid->medicionPrev=0.0;
	pid->out=0;

}

float PIDController_Update(PIDController *pid, float setpoint, float measurement){

	// Señal de error

	float error1;
	error1= setpoint - measurement;


	// Termino proporcional
	float proportional = pid->Kp*error1;


	// Termino integral
	pid-> integrador= pid->integrador + 0.5*(pid->Ki)*(pid->T)*(error1+pid->errorprev);

	// Antiwindup integrador

	if((pid->integrador) > (pid->limMaxInt)){
		pid->integrador = pid->limMaxInt;
	}else if((pid->integrador) < (pid->limMinInt)){
		pid->integrador = pid->limMinInt;
	}

	// Termino Derivativo

	/*

	pid->derivador = -(2.0 * pid->Kd * (measurement - pid->medicionPrev)
                        + (2.0f * pid->tau - pid->T) * pid->derivador)
                        / (2.0f * pid->tau + pid->T);
	 */

	pid->derivador = (pid->Kd)*(error1-pid->errorprev)/pid->T;

    // Calculo la salida PID y la limito

    pid->out = pid->integrador + pid->derivador + proportional;

    if(pid->out > pid->limMax){
    	pid->out = pid->limMax;
    }else if(pid->out < pid->limMin){
    	pid->out = pid->limMin;
    }

    // Guardo el error y la medicion para el proximo calculo

    pid->errorprev = error1;
    pid->medicionPrev = measurement;

    // Devuelvo la salida del controlador

    return pid->out;

}
