package main

/*
#cgo LDFLAGS: -L/opt/sw/guamps/dev/lib -lgmx -lguamps
#include "guamps/guampsio.h"
*/
import "C"
import "fmt"

// type Tpr_t struct {
// 	natoms int
// 	inputrec C.t_inputrec
// 	state C.t_state
	
//   int natoms;
//   t_inputrec inputrec;
//   t_state state;
//   rvec *f;
//   gmx_mtop_t mtop;
// } tpr_t

func LoadTpr(path string) (*C.tpr_t) {
	tpr := C.guamps_load_tpr(C.CString(path))
	return tpr
}

func WriteTpr(path string, tpr *C.tpr_t) {
	C.guamps_write_tpr(C.CString(path), tpr)
}

func main() {
	fmt.Printf("Hello, 世界\n")
	tpr := LoadTpr("topol.tpr")
	WriteTpr("new.tpr", tpr)
	
}

