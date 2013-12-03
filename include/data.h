#ifndef __GUAMPS_DATA_H__
#define __GUAMPS_DATA_H__

typedef enum {
  INT_T, FLOAT_T, DOUBLE_T, // scalar
  RVEC_T, // vectors
} type_t;

static const char *GUAMPS_TYPE_NAMES[] =  {
  [INT_T]="INT_T", [FLOAT_T]="FLOAT_T", [DOUBLE_T]="DOUBLE_T",
  [RVEC_T]="RVEC_T",
};


typedef struct {
  int length;
  rvec *rvec;
} rvec_t

typedef union {
  int v_int; float v_float; double v_double;
  rvec_t v_rvec;
} value_t

typedef struct {
  type_t type;
  value_t value;
} data_t


typedef enum {
  TPR_K, CPT_K, TRR_K
} selkind_t;

static const char *GUAMPS_SELKIND_NAMES[] = {
  [TPR_T]="TPR_T", [CPT_T]="CPT_T", [TRR_T]="TRR_T"
}

typedef struct {
  selkind_t kind;
  payload_t data;
} selectable_t;


#endif
