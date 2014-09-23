#include "data.h"

type_t* GUAMPS_TYPE(const char* str) {
  type_t* t;
  if      (0 == strcmp(str, "INT_T"))   { t = INT_T;   }
  else if (0 == strcmp(str, "LLINT_T")) { t = LLINT_T; }
  else if (0 == strcmp(str, "FLOAT_T")) { t = FLOAT_T; }
  else if (0 == strcmp(str, "DOUBLE_T")){ t = DOUBLE_T;}
  else if (0 == strcmp(str, "REAL_T"))  { t = REAL_T;  }
  else if (0 == strcmp(str, "RVEC_T"))  { t = RVEC_T;  }
  else if (0 == strcmp(str, "MATRIX_T")){ t = MATRIX_T;}
  else if (0 == strcmp(str, "ARRAY_T")) { t = ARRAY_T; }
  else {
    guamps_error("GUAMPS_TYPE: Unknown type %s\n", str);
  }
  return t;
}

bool guamps_is_simple_type(const type_t type) {
  switch (type) {
  case INT_T:
  case LLINT_T:
  case FLOAT_T:
  case DOUBLE_T:
  case REAL_T:
    return true;
    break;

  default:
    return false;
  }
}

char* guamps_type_format(const type_t type) {
  if (!guamps_is_simple_type(type)) {
    guamps_error("guamps_type_format: %s is not known as a simple type\n",
		 GUAMPS_TYPE_NAMES[type]);
    return NULL;
  }

  return GUAMPS_TYPE_SPEC[type];
}

void * guamps_calloc_simple(const type_t type) {
  if (!guamps_is_simple_type(type)) {
    guamps_error("guamps_calloc_simple: Expected simple type by got %s\n",
		 GUAMPS_TYPE_NAMES[type]);
    return NULL;
  }

  void* value;
  switch (type) {
  case INT_T:
    value = calloc(sizeof(int),1); break;
  case LLINT_T:
    value = calloc(sizeof(long long int), 1); break;
  case FLOAT_T:
    value = calloc(sizeof(float), 1); break;
  case DOUBLE_T:
    value = calloc(sizeof(double), 1); break;
  case REAL_T:
    value = calloc(sizeof(real), 1); break;

  default:
    guamps_error("guamps_calloc_simple: Unknown simple type %s\n",
		 GUAMPS_TYPE_NAMES[type]);
    break;
  }

  return value;
}

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

  case ARRAY_T:
    return data->value.v_array.array;
    break;

  }

}

array_t * guamps_array_create(const int length, const type_t type) {
  array_t * array = (array_t*)calloc(sizeof(array_t), 1);
  array->length = length;
  array->type   = type;

  switch(type) {
  case REAL_T:
    array->array = (real*)calloc(sizeof(real), length);
    break;

  default:
    guamps_error("guamps_array_new: Unknown array type %s\n", GUAMPS_TYPE_NAMES[type]);
    return NULL;
  }

  return array;

}

void * guamps_array_get(const array_t* array, const int i) {
  switch (array->type) {
  case REAL_T:
    return &((real*)array->array)[i];
    break;

  default:
    guamps_error("guamps_array_get: Unknown array type %s\n", GUAMPS_TYPE_NAMES[array->type]);
    return NULL;
 }
}

bool guamps_array_set(const array_t* array, const int i, const void * value) {
  switch(array->type) {
  case REAL_T:
    ((real*)array->array)[i] = *(real*)value;
    break;

  default:
    guamps_error("guamps_array_set: Unknown array type %s\n", GUAMPS_TYPE_NAMES[array->type]);
    return false;
  }
}
