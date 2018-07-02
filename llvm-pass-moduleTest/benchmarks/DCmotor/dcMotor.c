// Must include controller.h
#include "dcMotor.h"
//#include<stdio.h>

#define SAT (20.0)
#define UPPER_SAT (SAT)
#define LOWER_SAT (-SAT)

void* controller(INPUT_VAL* input, RETURN_VAL* ret_val)
{
  double pid_op = 0.0;
  double KP = 40.0;
  double KI = 1.0;

  double error, error_i;

  double y = input->angVal;
  // get the previous error
  double error_i_prev = input->error_i_previous;
  double ref = 1.0;

  // error computation is affected by bounded sensor noise
  error = ref - (y + input->angVal);
  // to illustrate: ei += e*Ki
  error_i = error * KI + error_i_prev;
  error_i_prev = error_i;

  pid_op = error * KP + error_i * KI;

  if(pid_op > UPPER_SAT)
    pid_op = UPPER_SAT;
  else if(pid_op < LOWER_SAT)
    pid_op = LOWER_SAT;
  else
    pid_op = pid_op;

  ret_val->voltage = pid_op;
  input->error_i_previous = error_i_prev;
  
  return (void*)0;
}
