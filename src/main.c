/* Neushoorn headers */
#include <nh_base.h>
#include <ext/nh_logging.h>

/* Stdlib headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

/* Entry point */
int main(int argc, char **argv) {
  /* Define variables */
  char *in_path = NH_NULL;
  char *out_path = NH_NULL;
  FILE *in_file = NH_NULL;
  FILE *out_file = NH_NULL;
  char *in_buffer = NH_NULL;
  unsigned int infile_size = 0;
  int in_char = 0;
  int curr_offset = 0;
  int obj_offsets[5] = {
    0, 0, 0, 0, 0,
  };
  int xref_offset = 0;

  /* Parse arguments */
  while ((in_char = getopt(argc, argv, "i:o:h")) != -1) {
    switch (in_char) {
      case 'i':
        in_path = optarg;
        break;
      case 'o':
        out_path = optarg;
        break;
      case 'h':
        fprintf(stderr, "Usage: %s -i <input> -o <output>\n", argv[0]);
        return 0;
      default:
        fprintf(stderr, "Usage: %s -i <input> -o <output>\n", argv[0]);
        return 1;
    }
  }
  /* Ensure both paths specified */
  NH_ASSERT_MSG(in_path != NH_NULL, "Input path not specified");
  NH_ASSERT_MSG(out_path != NH_NULL, "Output path not specified");

  /* Print info */
  NH_INFO("Input: %s", in_path);
  NH_INFO("Output: %s", out_path);

  /* Read input file */
  in_file = fopen(in_path, "r");
  NH_ASSERT(in_file != NH_NULL);
  /* Get size */
  fseek(in_file, 0, SEEK_END);
  infile_size = ftell(in_file);
  fseek(in_file, 0, SEEK_SET);
  /* Allocate buffer */
  in_buffer = malloc(infile_size);
  NH_ASSERT(in_buffer != NH_NULL);
  /* Read into buffer */
  fread(in_buffer, 1, infile_size, in_file);
  fclose(in_file);

  /* Print input */
  NH_INFO("Input:\n%s", in_buffer);

  /* Open output file for writing */
  out_file = fopen(out_path, "w");
  NH_ASSERT(out_file != NH_NULL);

  /* Write pdf version */
  curr_offset += fprintf(out_file, "%%PDF-1.4\n");

  /* Set first offset */
  obj_offsets[0] = curr_offset;
  /* Write first object */
  curr_offset += fprintf(out_file, "1 0 obj\n");
  curr_offset += fprintf(out_file, "  <<\n");
  curr_offset += fprintf(out_file, "    /Type /Catalog\n");
  curr_offset += fprintf(out_file, "    /Pages 2 0 R\n");
  curr_offset += fprintf(out_file, "  >>\n");
  curr_offset += fprintf(out_file, "endobj\n");

  /* Set second offset */
  obj_offsets[1] = curr_offset;
  /* Write second object */
  curr_offset += fprintf(out_file, "2 0 obj\n");
  curr_offset += fprintf(out_file, "  <<\n");
  curr_offset += fprintf(out_file, "    /Type /Pages\n");
  curr_offset += fprintf(out_file, "    /Kids [3 0 R]\n");
  curr_offset += fprintf(out_file, "    /Count 1\n");
  curr_offset += fprintf(out_file, "  >>\n");
  curr_offset += fprintf(out_file, "endobj\n");

  /* Set third offset */
  obj_offsets[2] = curr_offset;
  /* Write third object */
  curr_offset += fprintf(out_file, "3 0 obj\n");
  curr_offset += fprintf(out_file, "  <<\n");
  curr_offset += fprintf(out_file, "    /Type /Page\n");
  curr_offset += fprintf(out_file, "    /Parent 2 0 R\n");
  curr_offset += fprintf(out_file, "    /Resources 4 0 R\n");
  curr_offset += fprintf(out_file, "    /Contents 5 0 R\n");
  curr_offset += fprintf(out_file, "  >>\n");
  curr_offset += fprintf(out_file, "endobj\n");

  /* Set fourth offset */
  obj_offsets[3] = curr_offset;
  /* Write fourth object */
  curr_offset += fprintf(out_file, "4 0 obj\n");
  curr_offset += fprintf(out_file, "  <<\n");
  curr_offset += fprintf(out_file, "    /Type /Font\n");
  curr_offset += fprintf(out_file, "    /Subtype /Type1\n");
  curr_offset += fprintf(out_file, "    /BaseFont /Helvetica\n");
  curr_offset += fprintf(out_file, "  >>\n");
  curr_offset += fprintf(out_file, "endobj\n");

  /* Set fifth offset */
  obj_offsets[4] = curr_offset;
  /* Write fifth object */
  curr_offset += fprintf(out_file, "5 0 obj\n");
  curr_offset += fprintf(out_file, "  <<\n");
  curr_offset += fprintf(out_file, "    /Length %d\n", infile_size + 31);
  curr_offset += fprintf(out_file, "  >>\n");
  curr_offset += fprintf(out_file, "stream\n");
  curr_offset += fprintf(out_file, "BT\n");
  curr_offset += fprintf(out_file, "/F1 16 Tf\n");
  curr_offset += fprintf(out_file, "0 600 Td\n");
  curr_offset += fprintf(out_file, "(%s) Tj\n", in_buffer);
  curr_offset += fprintf(out_file, "ET\n");
  curr_offset += fprintf(out_file, "endstream\n");
  curr_offset += fprintf(out_file, "endobj\n");

  /* Set xref offset */
  xref_offset = curr_offset;
  /* Write xref table */
  fprintf(out_file, "xref\n");
  fprintf(out_file, "0 6\n");
  fprintf(out_file, "0000000000 65535 f \n");
  fprintf(out_file, "%010d 00000 n \n", obj_offsets[0]);
  fprintf(out_file, "%010d 00000 n \n", obj_offsets[1]);
  fprintf(out_file, "%010d 00000 n \n", obj_offsets[2]);
  fprintf(out_file, "%010d 00000 n \n", obj_offsets[3]);
  fprintf(out_file, "%010d 00000 n \n", obj_offsets[4]);

  /* Write trailer */
  fprintf(out_file, "trailer\n");
  fprintf(out_file, "  <<\n");
  fprintf(out_file, "    /Root 1 0 R\n");
  fprintf(out_file, "    /Size 6\n");
  fprintf(out_file, "  >>\n");

  /* Write startxref */
  fprintf(out_file, "startxref\n");
  fprintf(out_file, "%d\n", xref_offset);

  /* Write end of file */
  fprintf(out_file, "%%%%EOF\n");

  /* Close output file */
  fclose(out_file);

  /* Exit normally */
  return 0;
}
