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
} rvec_t;

typedef union {
  int v_int; float v_float; double v_double;
  rvec_t v_rvec;
} value_t;

typedef struct {
  type_t type;
  value_t value;
} data_t;


typedef enum {
  TPR_F, CPT_F, TRR_F
} filetype_t;

static const char *GUAMPS_FILETYPE_NAMES[] = {
  [TPR_F]="TPR_F", [CPT_F]="CPT_F", [TRR_F]="TRR_F"
};

typedef union {
  tpr_t p_tpr;
  cpt_t p_cpt;
  trr_t p_trr;
} payload_t;

typedef struct {
  filetype_t kind;
  payload_t data;
} selectable_t;


#endif
