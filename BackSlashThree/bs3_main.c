#include <stdlib.h>
#include <stdio.h>

#if defined(BS3RUNTIME)
/* RUNTIME main */
#include "bs3.h"
int main(int argc, char *argv[]) 
{
  int i, err;
  struct bs3_asm_code_map codemap;
  bs3_asm_code_map_reset(&codemap);
  for (i = 0 ; i < argc; i++)
  {
    if ((err = bs3_asm_code_map_load(argv[i],&codemap,0)) != BS3_ASM_CODE_MAP_ERR_OK)
    {
      if (err == BS3_ASM_CODE_MAP_ERR_LOADBADFILE && i == 0  && argc > 1) continue;
      fprintf(stderr,"Error on loading %s : %s\n",argv[i],bs3_asm_code_map_message[err]);
      return 1;
    }
  }
  bs3_hyper_main(&codemap,((void *)0)); /* Execute code with no debugger handler */
  return 0;
}

#else

/* ASSEMBLER / DEBUGGER  main */
#include <getopt.h>
#include <string.h>
#include "bs3_asm.h"
#include "bs3_debug.h"

void bs3_usage(const char * prgname )
  {
    printf("Usage: bs3asm [OPTION]... FILE                                                 \n");
    printf("Assembler / Debugger of BackSlashThree (BS3) CPU program.                      \n");
    printf("                                                                               \n");
    printf("  -b, --output-type-bin      Generated BS3 result file in binary format        \n");
    printf("  -d, --debug                Debug result program after successful assembling  \n");
    printf("  -e, --output-elf ELFFILE   Assembling result executable in ELFFILE           \n");
    printf("  -h, --help                 This current help message.                        \n");
    printf("  -i, --include-path         Path to search for include directive              \n");
    printf("  -o, --output-bs3 BS3FILE   Assembling BS3 result in BS3FILE                  \n");
    printf("  -p, --debug-port PORT      Remote debug listening port (default: 35853)      \n");
    printf("  -r, --output-report RFILE  Assembling report in RFILE                        \n");
    printf("  -x, --output-type-hexa     Generated BS3 result file in hexadecimal format   \n");
    printf("                                                                               \n");
    printf("If FILE has an .s, .asm, .S or .ASM extension, it assembles FILE, otherwise it \n");
    printf("load it for debug.                                                             \n");
    printf("After successful assembling, you can debug it right awway by using argument -d.\n");
    printf("Debugging port is always to localhost (127.0.0.1)                              \n");
    printf("Debug by using telnet : telnet localhost 35853                                 \n");
    printf("Remote debug allowed with SSH tunnel.                                          \n");
    printf("                                                                               \n");
    printf("Example:                                                                       \n");
    printf("   >bs3asm myprog.asm                                                          \n");
    printf("     locally generates 'out.bs3' binary file, and 'out.bs3.rpt' text report    \n");
    printf("                                                                               \n");
    printf("   >bs3asm -d -p 2300 myprog.asm                                               \n");
    printf("     If assembling completed successfully, then                                \n");
    printf("     Locally generates 'out.bs3' binary file, and 'out.bs3.rpt' text report.   \n");
    printf("     Starts debug on port 2300 of execution of 'out.bs3'                       \n");
    printf("                                                                               \n");
  }

#define BS3_MAX_FILENAME_SIZE 256
#define BS3_MAX_INCLUDEPATH 32

int main(int argc, char *argv[]) 
{
  char includePath[BS3_MAX_INCLUDEPATH][BS3_MAX_FILENAME_SIZE];
  int nbIncludePath;
  int toDebug;
  int toCompile;
  char outputBS3[BS3_MAX_FILENAME_SIZE];
  char outputReport[BS3_MAX_FILENAME_SIZE];
  char outputELF[BS3_MAX_FILENAME_SIZE];
  char inputFile[BS3_MAX_FILENAME_SIZE];
  int port;
  int c;
  int bs3type = BS3_ASM_CODE_MAP_TYPE_BINARY;
  static const struct option long_options[] =
  {
    {"include-path",      1, 0, 'i'},
    {"output-type-hexa",  0, 0, 'x'},
    {"output-type-bin",   0, 0, 'b'},
    {"output-bs3",        1, 0, 'o'},
    {"output-report",     1, 0, 'r'},
    {"output-elf",        1, 0, 'e'},
    {"debug",             0, 0, 'd'},
    {"debug-port",        1, 0, 'p'},
    {"help",              0, 0, 'h'},
    {0, 0, 0, 0}
  };
  
  strcpy(outputBS3,"out.bs3");
  strcpy(outputReport,"out.bs3.rpt");
  strcpy(outputELF,"");
  port = 0;
  nbIncludePath = 0;
  toDebug = 0;
  toCompile = 0;
  includePath[0][0] = 0xFF; /* -1 is end of list of include */
  /* getopt_long stores the option index here. */
  int option_index;
  while (1)
  {
    option_index = 0;
    c = getopt_long (argc, argv, ":i:o:r:e:p:hbxd",
                      long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
      {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;

      case 'd':
        toDebug = 1;
        break;

      case 'h':
        bs3_usage(argv[0]);
        return 0;
        break;

      case 'b':
        bs3type = BS3_ASM_CODE_MAP_TYPE_BINARY;
        break;
      case 'x':
        bs3type = BS3_ASM_CODE_MAP_TYPE_HEXA;
        break;

      case 'i':
        strncpy(includePath[nbIncludePath++], optarg, BS3_MAX_FILENAME_SIZE-1);
        includePath[nbIncludePath-1][BS3_MAX_FILENAME_SIZE-1] = 0;
        includePath[nbIncludePath][0] = 0xFF;
        includePath[nbIncludePath][1] = 0;
        break;

      case 'o':
        strncpy(outputBS3, optarg,  BS3_MAX_FILENAME_SIZE-1);
        outputBS3[BS3_MAX_FILENAME_SIZE-1] = 0;
        break;

      case 'r':
        strncpy(outputReport, optarg,  BS3_MAX_FILENAME_SIZE-1);
        outputReport[BS3_MAX_FILENAME_SIZE-1] = 0;
        break;
      case 'e':
        strncpy(outputELF, optarg,  BS3_MAX_FILENAME_SIZE-1);
        outputELF[BS3_MAX_FILENAME_SIZE-1] = 0;
        break;
      case 'p':
        port  = atoi(optarg);
        break;
      case '?':
      case ':':
        /* getopt_long already printed an error message. */
        printf("Incorrect provided parameter\n");
        bs3_usage(argv[0]);
        return 1;
        break;

      default:
        exit(1);
      }
  }

  if ((argc - optind) > 1)
  {
    printf("Too many parameters\n");
    bs3_usage(argv[0]);
    return 1;
  }
  if ((argc - optind) == 1)
  {
      strcpy(inputFile, argv[optind]);
  }
  else
  {
    printf("Missing BS3 assembly source or binary file parameter\n");
    bs3_usage(argv[0]);
    return 1;
  }

  struct bs3_asm_code_map codemap;
  
  if (toCompile)
  {
    if (bs3_asm_file(inputFile, outputBS3, outputReport, bs3type ) != BS3_ASM_PASS1_PARSE_ERR_OK) return 1;
    if (outputELF[0]!= 0) 
    {
      /* TODO Generate the ELF file */
    }
  }
  if (toDebug)
  {
    bs3_asm_code_map_reset(&codemap);
    bs3_asm_code_map_load(outputBS3, &codemap,0);
    bs3_debug_prepare(port); /* 0 for default port 35853*/
    bs3_hyper_main(&codemap,&bs3_debug);
    bs3_debug_end();
  }
  return 0;
 
}
#endif