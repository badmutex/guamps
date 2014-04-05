#include "guampsio.h"
#include "gmxutil.h"

#include "stdio.h"
#include "stdbool.h"
#include <string.h>
#include <getopt.h>

typedef struct {
  char *file;
  char *select;
  char *output;
  args_file_t *input;
  bool overwrite;
} arguments_t;

arguments_t* new_arguments_t() {
  arguments_t *a = (arguments_t*)calloc(1, sizeof(arguments_t));
  a->input = new_args_file_t();
  return a;
}

static struct option options[] = {
  {"file",   required_argument, 0,  'f' },
  {"select", required_argument, 0,  's' },
  {"input",  optional_argument, 0,  'i' },
  {"output", optional_argument, 0,  'o' },
  {"overwrite", no_argument,    0,  'O' },
};

void print_usage(FILE *stream, char *progname) {

  // Usage
  fprintf(stream,
	  "Usage: %s -f/--file FILE -s/--select STR [-i/--input FILE] [-o/--output FILE]\n", progname);

  // -f/--file
  fprintf(stream,
	  "    -f  --file FILE           Read from this file\n");

  // -s/--select
  fprintf(stream,
	  "    -s  --select              Select this from the FILE. Options are:\n"
	  );
  fprint_enum(GUAMPS_SELECTOR_NAMES, selector_t_count, 35, stream);

  // -i/--input
  fprintf(stream,
	  "    -i  --input FILE          Optional. If given, read the value(s) from this file [default=stdin].\n");

  // -o/--output
  fprintf(stream,
	  "    -o  --output FILE         Optional. If given, write the output to this file, else update --file.\n");

  // -O/--overwrite
  fprintf(stream,
	  "    -O  --overwrite           Overwrite the --file if present\n");

  // -h/--help
  fprintf(stream,
	  "    -h  --help                Print this usage text\n");

}


arguments_t * parse_opts(int argc, char *argv[], struct option options[]){

  if (argc <= 1) { return NULL; }

  arguments_t *args = new_args_file_t(); //(arguments_t *)malloc(sizeof(arguments_t));
  args->input = (args_file_t *)malloc(sizeof(args_file_t));

  /* defaults */
  args->input->type = FILETYPE_FILE;
  args->input->file.handle = stdin;
  args->overwrite = false;

  /* parse */
  while (true) {

    int option_index = 0;
    int c = getopt_long(argc, argv, "f:s:i:o:Oh?", options, &option_index);

    if(c == -1) break;

    switch(c) {
    case 'f':
      args->file = strdup(optarg);
      args->output = args->file;
      break;
    case 's':
      args->select = strdup(optarg);
      break;
    case 'i':
      args->input->type = FILETYPE_PATH;
      args->input->file.path = strdup(optarg);
      break;
    case 'o':
      args->output = strdup(optarg);
      break;
    case 'O':
      args->overwrite = true;
      break;

    case 'h':
    case '?':
    default:
      return NULL;
    }
  }

  return args;

}


int main(int argc, char *argv[]) {

  const char *progname = argv[0];

  arguments_t *args = parse_opts(argc, argv, options);
  if(args == NULL) {
    print_usage(stderr, argv[0]);
    return 1;
  }

  if (!guamps_gmx_set_progname(progname)) {
    fprintf(stderr, "Unable to initialize GROMACS\n");
    return 1;
  }

  data_t data;
  selector_t sel;
  selectable_t *obj;
  type_t type;

  obj = guamps_load(args->file);
  if(!guamps_pick_selector(args->select, &sel)){ return 1; }
  type = guamps_selector_type(obj->kind, sel);

  FILE *fh;
  if(!(fh = args_file_fopen(args->input, "r"))) {
    guamps_error("Failed to open %s for reading\n", args->input);
    return 1;
  }

  if(!guamps_fread(fh, type, &data)){
    guamps_error("Cannot read data\n");
    return 1;
  }

  if(!guamps_update(obj, sel, &data)){
    guamps_error("Cannot update %s with new values\n", GUAMPS_FILETYPE_NAMES[obj->kind]);
    return 1;
  }
  
  if(args->overwrite && access(args->output, W_OK) == 0){
    unlink(args->output);
  }

  if(!guamps_write(args->output, obj)){
    guamps_error("Failed to write %s\n", args->output);
    return 1;
  }

}
