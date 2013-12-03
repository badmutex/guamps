#ifndef __GUAMPS_IO_GET_H__
#define __GUAMPS_IO_GET_H__

#include "file_t.h"
#include "gmx_t.h"

/* *********************************************************************
   Reading from GROMACS files
 ***********************************************************************/
selectable_t *guamps_load(const char *path);
cpt_t * guamps_load_cpt(const char *path);
tpr_t * guamps_load_tpr(const char *path);
trr_t * guamps_load_trr(const char *path);

/* *********************************************************************
   Extracting data
 ***********************************************************************/
bool guamps_select    (const *selectable, const selector_t sel, data_t *res);
bool guamps_select_cpt(const cpt_t *cpt , const selector_t sel, data_t *res);
bool guamps_select_tpr(const tpr_t *tpr , const selector_t sel, data_t *res);
bool guamps_select_trr(const trr_t *trr , const selector_t sel, data_t *res);

/* *********************************************************************
   Writing to files
 ***********************************************************************/
bool guamps_write_data(file_t *fh, const data_t *data);
bool guamps_write_tpr(const char *path, const tpr_t *tpr);
bool guamps_write_cpt(const char *path, const cpt_t *cpt);
bool guamps_write_scalar(file_t *fh, const data_t *data);
bool guamps_write_rvec(file_t *fh, const rvec *vec, const unsigned int count);

#endif
