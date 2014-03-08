#include "data.h"

void guamps_data_set(const type_t type, const void* val, data_t *data) {

  data->type = type;

  switch (type) {

  case INT_T:
    data->value.v_int = *(int*)val;
    break;

  case LLINT_T:
    data->value.v_llint = *(long long int*)val;
    break;

  case FLOAT_T:
    data->value.v_float = *(float*)val;
    break;

  case DOUBLE_T:
    data->value.v_double = *(double*)val;
    break;

  case REAL_T:
    guamps_data_set(GMX_REAL_TYPE_T, val, data);
    break;

  case RVEC_T:
    data->value.v_rvec = *(rvec_t*)val;
    break;

  case MATRIX_T:

    for (int i=0; i<3; i++){
      for (int j=0; j<3; j++){
	data->value.v_matrix[i][j] = (*(matrix*)val)[i][j];
      }
    }
    /* data->value.v_matrix = *(matrix**)val; */
    break;

  }

}

void * guamps_data_get(const data_t *data) {

  /*
    based on the type of data cast dst to the appropriate typed
    pointer then dereference for assignment.

    eg: data->type is INT_T so
    *(int*)dst = data->value.v_int
   */

  type_t type = data->type;
  value_t value = data->value;

  switch (type) {

  case INT_T:
    return (void *)&data->value.v_int;
    break;

  case LLINT_T:
    return (void *)&data->value.v_llint;
   break;

  case FLOAT_T:
    return (void *)&data->value.v_float;
    break;

  case DOUBLE_T:
    return (void *)&data->value.v_double;
    break;

  case REAL_T:
    guamps_error("guamps_data_get: I don't know how to get a REAL_T value\n");
    return NULL;
    break;

  case RVEC_T:
    return (void *)&data->value.v_rvec;
    break;

  case MATRIX_T:
    return (void *)&data->value.v_matrix;
    break;

  }

}
