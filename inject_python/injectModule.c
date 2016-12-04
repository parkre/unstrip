#include <Python.h>
#include "libelfsh.h"

#define BUF_SIZE 1024

int ARR_SIZE = 4098;
int get_sect_index(char *);
int get_binding(char);

int CinjectSyms(char *in, char *syms, char *out_file) {
  elfshobj_t *in_file;
  FILE *syms_file;
  int ret;
  elfsh_Sym sym;
  int tot_syms = 0;
  char **name = (char**) malloc(ARR_SIZE * sizeof(char*));
  long int *addr = (long int*) malloc(ARR_SIZE * sizeof(long int));
  long int *size = (long int*) malloc(ARR_SIZE * sizeof(long int));
  int *section = (int *) malloc(ARR_SIZE * sizeof(int));
  int *binding = (int *) malloc(ARR_SIZE * sizeof(int));

  /* get input file from command line arguments */
  in_file = elfsh_map_obj(in);
  if(!in_file) {
    printf("Error loading input file: aborting...\n");
    elfsh_error();
    exit(1);
  }

  /* get the symbols file from command arguments */
  syms_file = fopen(syms, "r");
  if(!syms_file) {
    printf("Error loading symbols file: aborting...\n");
    elfsh_error();
    exit(1);
  }

  /* read symbols file and fill addr, size and type arrays */
  char *line = NULL;
  char temp[BUF_SIZE];
  int temp_indx;
  char *tok;
  size_t read;
  /* loop assumes correct input <name, 0x0000addr, size> */
  while((read = getline(&line, &read, syms_file)) != -1) {
      // if arrays are going to be overrun, double lengths
      if(tot_syms >= ARR_SIZE) {
	  printf("growing...\n");
	  ARR_SIZE *= 2;
	  name = (char **) realloc(name, ARR_SIZE * sizeof(char*));
	  addr = (long int *) realloc(addr, ARR_SIZE * sizeof(long int));
	  size = (long  int *) realloc(size, ARR_SIZE * sizeof(long int));
	  section = (int *) realloc(section, ARR_SIZE * sizeof(int));
	  binding = (int *) realloc(binding, ARR_SIZE * sizeof(int));
      }

      /* prep line */
      strcpy(temp, line);
      temp_indx = 0;
      while(temp[temp_indx + 1]) {
	  temp[temp_indx] = temp[temp_indx + 1];
	  temp_indx++;
      }
      temp[strlen(temp) - 3] = '\0';

      /* get symbol name */
      tok = strtok(temp, ",");
      name[tot_syms] = (char*) malloc(strlen(tok) + 1);
      strcpy(name[tot_syms],tok);
      
      /* get symbol address */
      tok = strtok(NULL, ",");
      addr[tot_syms] = strtol(tok, NULL, 0);
      //printf("%li\n", addr[tot_syms]);
      
      /* get symbol size */
      tok = strtok(NULL, ",");
      size[tot_syms] = strtol(tok, NULL, 0);
      //printf("%li\n", size[tot_syms - 1]);
      
      /* get section */
      tok = strtok(NULL, ",");
      section[tot_syms] = get_sect_index(tok);
   
      /* get binding */
      tok = strtok(NULL, ",");
      binding[tot_syms++] = get_binding(tok[1]);
  }

  //printf("---------------------------------------------------------");
  int i;
  /*for(i = 0; i < tot_syms; i++) {
    printf("%s, 0x%li, %li\n", name[i], addr[i], size[i]);
    }*/

  printf("injecting symbols...\n");
  /* get the symbol information */
  for(i = 0; i < tot_syms; i++) {
    sym = elfsh_create_symbol(addr[i], size[i], STT_FUNC, binding[i], 0, section[i]);
    ret = elfsh_insert_symbol(in_file->secthash[ELFSH_SECTION_SYMTAB], &sym, name[i]);
    if(ret < 0) {
      elfsh_error();
      exit(-1);
    }
  }

  ret  = elfsh_save_obj(in_file, out_file);
  if(ret < 0) {
    elfsh_error();
  }

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return tot_syms;
}

/* this function returns the correct index for the injection process */
int get_sect_index(char * sect_name) {
    if(!strcmp(sect_name, " .text")) {
	return 6;
    }
    else if(!strcmp(sect_name, " __libc_freeres_fn")) {
	return 7;
    }
    else if(!strcmp(sect_name, " __libc_thread_freeres_fn")) {
    	return 8;
    }
    else if(!strcmp(sect_name, " .fini")) {
	return 9;
    }
    else
	return 0;
}

int get_binding(char c) {
    if(c == 'l') {
	return 0;
    }
    else if(c == 'g') {
	return 1;
    }
    else {
	return 3;
    }
}

/*
 * injectSyms() : a python wrapper function for CinjectSyms
 *                takes in three arguments
 */
static PyObject* injectSyms(PyObject *self, PyObject *args) {
  char *infile;
  char *symfile;
  char *outfile;

  /* parse tuple to get arguments */
  if(!PyArg_ParseTuple(args, "sss", &infile, &symfile, &outfile)) {
    return NULL;
  }

  int i = CinjectSyms(infile, symfile, outfile);
  if(!i) {
    printf("Well crap...\n");
    return NULL;
  }
  else {
    return Py_BuildValue("i", i);
  }
}

/* function table for Python name bindings */
static PyMethodDef inject_methods[] = {
  {"injectSyms", injectSyms, METH_VARARGS, "Arguments: <stripped_binary> <symbol_file> <new_executable> :: Injects symbols from symbol file into the stripped binary and creates a new executable"},
  {NULL, NULL, 0, NULL}
};

/* initialization function, called when compiling and build */
PyMODINIT_FUNC initinject(void) {
  //  PyObject *m;
  (void) Py_InitModule("inject", inject_methods);
  //if(m == NULL) return;

  
  //injectError = PyErr_NewException("inject.error", NULL, NULL);
  //Py_INCREF(injectError);

  //PyModule_AddObject(m, "error", Py_BuildValue("s", "InjectError"));
}
