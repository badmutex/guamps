#include "data.h"
#include "select.h"


const selector_t* guamps_selector_t_create(const selector_key key, const index_t* index) {
  selector_t* sel = (selector_t*)calloc(sizeof(selector_t), 1);
  sel->key = (selector_key)key;
  sel->index = (unsigned long long*)index;
  return sel;
}

type_t guamps_selector_type(const filetype_t ft, const selector_t sel) {
  switch(sel.key) {
  case TIME:
    switch(ft) {
    case TPR_F:
      return DOUBLE_T;
    case TRR_F:
      return GMX_REAL_TYPE_T;
    default:
      return GUAMPS_SELECTOR_TYPES[sel.key];
    }
  default:
    return GUAMPS_SELECTOR_TYPES[sel.key];
  }

}
