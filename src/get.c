#include "guampsio.h"
#include "gmxutil.h"
#include "output.h"

#include "stdio.h"
#include "stdbool.h"
#include <getopt.h>
#include <ctype.h>
#include <string.h>


typedef struct {
  char *file;
  char *select;
  args_file_t *output;
} arguments_t;

arguments_t* new_arguments_t() {
  arguments_t *a = (arguments_t*)calloc(1, sizeof(arguments_t));
  a->output = new_args_file_t();
  return a;
}

static struct option options[] = {
  {"file",   required_argument, 0,  'f' },
  {"select", required_argument, 0,  's' },
  {"output", optional_argument, 0,  'o' },
};

void print_usage(FILE *stream, char *progname) {

  // Usage
  fprintf(stream,
	  "Usage: %s -f/--file FILE -s/--select STR [-o/--output FILE]\n", progname);

  // -f/--file
  fprintf(stream,
	  "    -f  --file FILE           Read from this file\n");

  // -s/--select
  fprintf(stream,
	  "    -s  --select              Select this from the FILE. Options are:\n"
	  );
  fprint_enum(GUAMPS_SELECTOR_NAMES, selector_t_count, 35, stream);

  // -o/--output
  fprintf(stream,
	  "    -o  --output FILE         Optional. If given, write the output to this file.\n");

  // -h/--help
  fprintf(stream,
	  "    -h  --help                Print this usage text\n");

}


arguments_t * parse_opts(int argc, char *argv[], struct option options[]){

  if (argc <= 1) { return NULL; }

  arguments_t *args = (arguments_t *)malloc(sizeof(arguments_t));
  args->output = (args_file_t *)malloc(sizeof(args_file_t));

  /* defaults */
  args->output->type = FILETYPE_FILE;
  args->output->file.handle = stdout;

  /* parse */
  while (true) {

    int option_index = 0;
    int c = getopt_long(argc, argv, "f:s:o:h?", options, &option_index);

    if(c == -1) break;

    switch(c) {
    case 'f':
      args->file = strdup(optarg);
      break;
    case 's':
      args->select = strdup(optarg);
      break;
    case 'o':
      args->output->type = FILETYPE_PATH;
      args->output->file.path = strdup(optarg);
      break;

    case 'h':
    case '?':
    default:
      return NULL;
    }
  }

  return args;

}


int main(int argc, char *argv[]){

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

  data_t r;
  selector_t selector;

  if (!guamps_pick_selector(args->select, &selector)) {
    fprintf(stderr, "Unknown selection: %s\n", args->select);
    return 1;
  }

  data_t data;
  selectable_t *sel;

  if(!(sel = guamps_load(args->file))) {
    guamps_error("%s: failed to load %s\n", progname, args->file);
    return 1;
  }
  if(!guamps_select(sel, selector, &data)){
    guamps_error("%s: failed to select %s\n", progname, GUAMPS_SELECTOR_NAMES[selector]);
    return 1;
  }

  FILE *fh;
  if(!(fh = args_file_fopen(args->output, "w"))) {
    guamps_error("Failed to open %s for writing\n", args->output);
    return 1;
  }

  if(!guamps_fwrite(fh, &data)) {
    guamps_error("Failed to write data to %s\n", args->output);
    return 1;
  }

}
