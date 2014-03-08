#include "data.h"
#include "select.h"

type_t guamps_selector_type(const filetype_t ft, const selector_t sel) {
  switch(sel) {
  case TIME:
    switch(ft) {
    case TPR_F:
      return DOUBLE_T;
    case TRR_F:
      return GMX_REAL_TYPE_T;
    default:
      return GUAMPS_SELECTOR_TYPES[sel];
    }
  default:
    return GUAMPS_SELECTOR_TYPES[sel];
  }

}
