#ifndef __GUAMPS_GUAMPSIO_H__
#define __GUAMPS_GUAMPSIO_H__

#include "file.h"
#include "gmx_t.h"
#include "select.h"
#include "data.h"
#include "output.h"

#include "stdbool.h"

/* *********************************************************************
   Reading from GROMACS files
 ***********************************************************************/
selectable_t *guamps_load(const char *path);
cpt_t * guamps_load_cpt(const char *path); // TODO
tpr_t * guamps_load_tpr(const char *path);
trr_t * guamps_load_trr(const char *path);

/* *********************************************************************
   Extracting data
 ***********************************************************************/
bool guamps_select    (const selectable_t *data, const selector_t sel, data_t *res);
bool guamps_select_cpt(const cpt_t *cpt , const selector_t sel, data_t *res);
bool guamps_select_tpr(const tpr_t *tpr , const selector_t sel, data_t *res);
bool guamps_select_trr(const trr_t *trr , const selector_t sel, data_t *res);


/* *********************************************************************
   Writing to files
 ***********************************************************************/
bool guamps_write(FILE *fh, const data_t *data);
bool guamps_write_tpr(const char *path, const tpr_t *tpr); // TODO
bool guamps_write_cpt(const char *path, const cpt_t *cpt); // TODO
bool guamps_write_scalar(FILE *fh, const data_t *data);
bool guamps_write_rvec(FILE *fh, const rvec *vec, const int count);

/* *********************************************************************
   Parsing selection string from user
 ***********************************************************************/
bool guamps_pick_selector(const char *str, selector_t *sel);
bool guamps_selector_type(const selector_t sel, type_t *type);
bool guamps_pick_filetype(const char *path, filetype_t *ftype);

#endif
