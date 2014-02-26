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

  default:
    guamps_error("guamps_data_set: I don't know how to set type %s\n", GUAMPS_TYPE_NAMES[type]);
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

  case RVEC_T:
    return (void *)&data->value.v_rvec;
    break;

  }

}
