
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "droste.h"

typedef struct {
    uint16 fileType;    /* File type id (always 0x4d42 "BM") */
    uint32 fileSize;    /* Size of the file in bytes */
    uint16 reserved1;   /* Always 0 */
    uint16 reserved2;   /* Always 0 */
    uint32 bitmapOffset;/* Starting position of image data in bytes */
} __attribute__((__packed__)) BitmapFileHeader;

typedef struct {
    uint32 size;        /* Size of the header in bytes */
    uint16 width;       /* Image width in pixels */
    uint16 height;      /* Image height in pixels */
    uint16 planes;
    uint16 bitsPerPixel;/* Number of bits per pixel */
} __attribute__((__packed__)) Bitmap2xHeaderStart;

typedef struct {
    uint32 size;        /* Size of the header in bytes */
    uint32 width;       /* Image width in pixels */
    uint32 height;      /* Image height in pixels */
    uint16 planes;
    uint16 bitsPerPixel;/* Number of bits per pixel */
    uint32 compression; /* Compression methods used */
} __attribute__((__packed__)) Bitmap3x4xHeaderStart;

uint32 bitmapOffset, bitsPerPixel;
uint32 fileSize;
Pixel *inBitmap, *outBitmap;


int check_2x_bitmap(FILE *fp, BitmapFileHeader *file_header, uint32 size) {
    Bitmap2xHeaderStart header;

    puts("Bitmap version 2.x");
    
    if (fread(&header.width, sizeof(header)-sizeof(size), 1, fp) != 1)
    {
        fputs("Couldn't read bitmap 2.x header\n", stderr);
        return 0;
    }

    width = header.width;
    height = header.height;
    bitmapOffset = file_header->bitmapOffset;
    bitsPerPixel = header.bitsPerPixel;

    return 1;
}

int check_3x4x_bitmap(FILE *fp, BitmapFileHeader *file_header, uint32 size) {
    Bitmap3x4xHeaderStart header;

    puts("Bitmap version 3.x/4.x");

    if (fread(&header.width, sizeof(header)-sizeof(size), 1, fp) != 1)
    {
        fputs("Couldn't read bitmap 3.x/4.x header\n", stderr);
        return 0;
    }

    if (header.compression != 0) {
        fputs("Can't read bitmap - please re-save without compression.\n", stderr);
        return 0;
    }

    width = header.width;
    height = header.height;
    bitmapOffset = file_header->bitmapOffset;
    bitsPerPixel = header.bitsPerPixel;

    return 1;
}

int check_bitmap(FILE *fp) {
    BitmapFileHeader header;
    uint32 size;

    if (fread(&header, sizeof(header), 1, fp) != 1) {
        fprintf(stderr, "Couldn't read bitmap file header: %d\n", size);
        return 0;
    }
    if (header.fileType != 0x4d42)
    {
        fputs("File is not a bitmap\n", stderr);
        return 0;
    }

    printf("File type: %x\n", header.fileType);
    printf("File size: %d\n", header.fileSize);
    fileSize = header.fileSize;
    
    if (fread(&size, sizeof(size), 1, fp) != 1) {
        fputs("Couldn't read bitmap header\n", stderr);
        return 0;
    }
    switch (size) {
    case 12:    /* v2.x Bitmap */
        return check_2x_bitmap(fp, &header, size);
    case 40:    /* v3.x Bitmap */
    case 108:   /* v4.x Bitmap */
        return check_3x4x_bitmap(fp, &header, size);
    default:
        fprintf(stderr, "Doesn't correspond to any known bitmap header size: %d\n", size);
        return 0;
    }
}

void put_info(void) {
    printf("Dimensions: %d x %d\nBitmap offset: 0x%x\nBPP: %d\n", width, height, bitmapOffset, bitsPerPixel);
}

void sprint_outfilename(char *outfile, char *infile) {
    char *dotpos;
    strcpy(outfile, infile);

    dotpos = strrchr(outfile, '.');
    if (dotpos == NULL)
        dotpos = &outfile[strlen(outfile)];

    strcpy(dotpos, "-droste.bmp");
    printf("Creating %s\n", outfile);
}

Pixel *alloc_bitmap(void) {
    return malloc(sizeof(Pixel) * width * height);
}

void read_bitmap(FILE *fp, Pixel *obmp) {
    int i = 0;

    fseek(fp, bitmapOffset, SEEK_SET);

    switch (bitsPerPixel) {
    case 24:
        {
            uint8 rgb[3];
            while (!feof(fp)) {
                if (fread(rgb, 3, 1, fp) != 1) {
                    if (feof(fp)) break;
                    fputs("Failed during read of 24bit bitmap\n", stderr);
                    exit(5);
                    return;
                }
                obmp[i++] = pixel(rgb[0], rgb[1], rgb[2]);
            }
        }
        break;
    case 32:
        {
            uint8 argb[4];
            while (!feof(fp)) {
                if (fread(argb, 4, 1, fp) != 1) {
                    if (feof(fp)) break;
                    fputs("Failed during read of 32bit bitmap\n", stderr);
                    exit(6);
                    return;
                }
                obmp[i++] = pixel(argb[1], argb[2], argb[3]);
            }
        }
        break;
    default:
        fprintf(stderr, "Unhandled depth: %d. Use 24 or 32 instead.\n", bitsPerPixel);
        exit(4);
    }
}

void write_bitmap(FILE *fp, Pixel *ibmp) {
    int i;

    fseek(fp, bitmapOffset, SEEK_SET);

    switch (bitsPerPixel) {
    case 24:
        {
            uint8 rgb[3];
            for (i = 0; i < width*height; i++) {
                rgb[0] = ibmp[i].r;
                rgb[1] = ibmp[i].g;
                rgb[2] = ibmp[i].b;
                if (fwrite(rgb, 3, 1, fp) != 1) {
                    fputs("Failed during write of 24bit bitmap\n", stderr);
                    exit(9);
                    return;
                }
            }
        }
        break;
    case 32:
        {
            uint8 argb[4];
            for (i = 0; i < width*height; i++) {
                argb[0] = 0;
                argb[1] = ibmp[i].r;
                argb[2] = ibmp[i].g;
                argb[3] = ibmp[i].b;
                if (fwrite(argb, 4, 1, fp) != 1) {
                    fputs("Failed during write of 32bit bitmap\n", stderr);
                    exit(10);
                    return;
                }
            }
        }
        break;
    default:
        fprintf(stderr, "Unhandled depth: %d. Use 24 or 32 instead.\n", bitsPerPixel);
        exit(8);
    }
}

void copy_headers(FILE *ofp, FILE *ifp) {
    char buf[1024];
    fseek(ifp, 0, SEEK_SET);
    fseek(ofp, 0, SEEK_SET);

    if (fread(buf, bitmapOffset, 1, ifp) != 1) {
        fputs("Couldn't copy bitmap headers", stderr);
        exit(6);
    }

    if (fwrite(buf, bitmapOffset, 1, ofp) != 1) {
        fputs("Couldn't copy bitmap headers", stderr);
        exit(7);
    }
}

int main(int argc, char *argv[]) {
    int show_eog = 0;

    if (argc == 3 && strcmp(argv[2], "-eog") == 0) {
        show_eog = 1;
        argc = 2;
    }

    if (argc != 2) {
        fputs("Usage: droste file.bmp\n", stderr);
        return 1;
    } else {
        int retval = 0;
        char *filename = argv[1];
        char out_fname[64];
        FILE *fp = fopen(filename, "r");

        if (fp == NULL) {
            perror(filename);
            return 2;
        }

        if (!check_bitmap(fp)) {
            retval = 3;
        } else {
            FILE *ofp;
            put_info();
            sprint_outfilename(out_fname, filename);
            ofp = fopen(out_fname, "w");
            if (ofp == NULL) {
                perror(out_fname);
            } else {
                inBitmap = alloc_bitmap();
                outBitmap = alloc_bitmap();

                copy_headers(ofp, fp);
                read_bitmap(fp, inBitmap);
                transform(outBitmap, inBitmap);
                write_bitmap(ofp, outBitmap);

                if (show_eog) {
                    char buf[256];
                    sprintf(buf, "eog %s", out_fname);
                    retval = system(buf);
                }

                fclose(ofp);
            }
        }

        fclose(fp);
        return retval;
    }
}
