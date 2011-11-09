/**
 * Converts color ppm picture given via stdin to either greyscale or sepia
 * based on command line parameters.
 *
 * USAGE: a.out <1|2>
 *      1 - convert to greyscale
 *      2 - convert to sepia
 *
 *
 * Chris Blades
 * 14 February, 20111
 */
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "safe.h"

#define GREY_PARAM 1
#define SEPIA_PARAM 2
#define PPM_COLOR_VERSION 6
#define COLOR_MAGIC_NUM "P5"
#define SEPIA_MAGIC_NUM "P6"
#define HEADER_ERROR 1
#define VERSION_ERROR 2
#define PARAM_ERROR 3
#define CORRUPT_ERROR 4
#define USAGE "Usage: prog2 <1|2>\n \
               1 - convert to greyscale\n \
               2 - convert to sepia.\n"

void convertToGreyscale(unsigned char *, unsigned char *, int);
void convertToSepia(unsigned char *, unsigned char *, int);

/**
 * Reads in header data and picture data from standard in and then calls 
 * functions to perform either greyscale or sepia conversion based on parameter.
 *
 * PARAMETERS:
 *  argc - the number of command line arguments, including the command
 *  argv - array of command line arguments, including the command
 *
 *  RETURN:
 *   Success if conversion was performed successfully, otherwise for usage, 
 *   corruption, or invalid format errors.
 */
int main(int argc, char **argv) {
    int            conversion;
    int            size;
    unsigned char *in         = NULL;
    unsigned char *out        = NULL;
    ppm_header    *header     = NULL;

    if (argc == 2) {
        conversion = atoi(argv[1]);
    } else {
        fprintf(stderr, USAGE);
        exit(PARAM_ERROR);
    }
    
    header = (ppm_header *)malloc(sizeof(ppm_header));
    readHeader(stdin, header);
   
    if (header == NULL) {
        fprintf(stderr, "Invalid header, exiting..\n");
        exit(HEADER_ERROR);
    } else if (header->version != PPM_COLOR_VERSION) {
        fprintf(stderr, "Header version must be P6, exiting.\n");
        exit(VERSION_ERROR);
    }
    size = header->width * header->height;
    
    in = (unsigned char *)smalloc(3 * size);
    int numRead = fread(in, sizeof(unsigned char), size * 3, stdin);

    if (numRead != size * 3) {
        fprintf(stderr, "Corrupt input file, quiting.\n");
        exit(CORRUPT_ERROR);
    }

    if (conversion == GREY_PARAM) {
        out = (unsigned char *)smalloc(size);
        convertToGreyscale(in, out, size);
        fprintf(stdout, "%s %d %d %d\n", COLOR_MAGIC_NUM, header->width,
                                         header->height, header->maxValue);
        fwrite(out, 1, size, stdout);
    } else if (conversion == SEPIA_PARAM) {
        out = (unsigned char *)smalloc(3 * size);
        convertToSepia(in, out, size);
        fprintf(stdout, "%s %d %d %d\n", SEPIA_MAGIC_NUM, header->width,
                                         header->height, header->maxValue);
        fwrite(out, 1, size * 3, stdout);
    } else {
        fprintf(stderr, "hey");
        exit(PARAM_ERROR);
    }

    free(header);
    return EXIT_SUCCESS;
}


/**
 * Convert ppm image data to greyscale.  Data is expected in format
 * <r><g><b> per pixel where each pixel takes up 3 bytes.
 *
 * Conversion is done with the formula:
 *  y = .299R + .587G + .114B
 *
 *  PARAMERTERS:
 *  in  - memory segment to read input data from
 *  out - memory segment to write image data to
 *  size- the size of in, in bytes
 */
void convertToGreyscale(unsigned char *in, unsigned char *out, int size) {
    unsigned char *pixelRGB;
    char           pixelGrey;
    int            i;

    for (i = 0; i < size; i++) {
        pixelRGB = in + (i * 3);

        pixelGrey = (.299 * pixelRGB[0]) + 
                    (.587 * pixelRGB[1]) + 
                    (.114 * pixelRGB[2]);

        out[i] = pixelGrey;
    }
}

/**
 * Converts the given image data into sepia tone.
 *
 * PARAMETERS:
 * in   - memory segment to read image data from
 * out  - memory segment to write image data to
 * size - size of in, in bytes
 */
void convertToSepia(unsigned char *in, unsigned char *out, int size) {
    unsigned char *pixelRGB;
    unsigned char *pixelSepia;
    int            sepiaR;
    int            sepiaG;
    int            sepiaB;
    int            i;

    for (i = 0; i < size; i++) {
        pixelRGB = in + (i * 3);
        pixelSepia = out + (i * 3);
    
        sepiaR = (.393 * pixelRGB[0]) + 
                 (.769 * pixelRGB[1]) + 
                 (.189 * pixelRGB[2]);

        sepiaG = (.349 * pixelRGB[0]) + 
                 (.686 * pixelRGB[1]) + 
                 (.168 * pixelRGB[2]);

        sepiaB = (.272 * pixelRGB[0]) + 
                 (.534 * pixelRGB[1]) + 
                 (.131 * pixelRGB[2]);
    
        sepiaR = sepiaR > 255 ? 255 : sepiaR;
        sepiaG = sepiaG > 255 ? 255 : sepiaG;
        sepiaB = sepiaB > 255 ? 255 : sepiaB;
    
        pixelSepia[0] = sepiaR;
        pixelSepia[1] = sepiaG;
        pixelSepia[2] = sepiaB;
    }
}

