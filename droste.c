#include <stdio.h>

#include <math.h>

#include "droste.h"

uint32 width, height;
Pixel transpColor;


Pixel pixel(uint8 r, uint8 g, uint8 b) {
    Pixel result = {r, g, b};
    return result;
}

int pixel_eq(Pixel x, Pixel y) {
    return x.r == y.r && x.g == y.g && x.b == y.b;
}

/* Variables that are constant across a single image */
static const double two_pi = 2.0 * M_PI;
static double origin_x, origin_y;
static double r1, r2, r2_over_r1, log_r1;
static double a, period, cosa, sina, cosacosa, cosasina;

/* Rotate the coordinates so that diagonal coincides with the imaginary
   axis and shrunk to be 2pi high.
    Where z = x+jy, and a = atan(log(r2/r1)/2pi),
    z' = z cos(a) e**ja
    or  x' = x cos(a) cos(a) - y cos(a) sin(a)
        y' = y cos(a) cos(a) + x cos(a) sin(a)
 */
void rotate(double *rx, double *ry, double x, double y) {
    *rx = x*cosacosa - y*cosasina;
    *ry = y*cosacosa + x*cosasina;
}

/* Transform x and y into logarithmic polar coordinates:
    where z = x+jy,
    z' = log(z)
    or  x' = log(sqrt(x*x + y*y))
        y' = atan(y/x)
 */
void to_logpolar(double *rx, double *ry, double x, double y) {
    *rx = log(sqrt(x*x + y*y)) - log_r1;
    *ry = atan2(y, x);
}

/* Transform log-polar coords x and y into cartesian coordinates.
    Where z = x+jy,
    z' = e**z
    or  x' = cos(y) e**x
        y' = sin(y) e**x */
void to_cartes(double *rx, double *ry, double x, double y) {
    double e_x = exp(x + log_r1);
    *rx = cos(y) * e_x;
    *ry = sin(y) * e_x;
}

void init_transform(Pixel *bmp) {
    transpColor = bmp[width*height/2 + width/2];
    printf("Transparent (hex color): #%02x%02x%02x\n",
        transpColor.r, transpColor.g, transpColor.b);

    /* Centre of the transformation, currently always at
       the centre of the image, but could really be anywhere */
    origin_x = width/2.0;
    origin_y = height/2.0;
    printf("Centre at (%f, %f)\n", origin_x, origin_y);

    /* Calculate r2_over_r1 (used to calculate the period of repetition).
       Currently assumes the origin is at the centre of the image.
       r2 is the outer radius (beyond which may lie transparent pixels).
       r1 is the inner radius (within which may lie transparent pixels). */
    r2 = origin_y < origin_x ? origin_y : origin_x;

    int i;
    r1 = 0.0;
    for (i = 0; i < width * height; i++) {
        if (pixel_eq(bmp[i], transpColor)) {
            double x = i % width - origin_x,
                   y = i / width - origin_y;
            double r = sqrt(x*x + y*y);
            if (r > r1)
                r1 = r;
        }
    }
    printf("r2 = %f, r1 = %f\n", r2, r1);
    
    r2_over_r1 = r2/r1;
    log_r1 = log(r1);

    /* Set up some values that are constant across the image */
    period = log(r2_over_r1);
    a = atan2(period, two_pi);
    cosa = cos(a);
    sina = sin(a);
    cosacosa = cosa*cosa;
    cosasina = cosa*sina;
}

void transform_droste(Pixel *obmp, Pixel *ibmp) {
    int i;
    int repeat_min = -2, repeat_max = 10;

    init_transform(ibmp);

    for (i = 0; i < width*height; i++) {
        int j, k;
        double x = i % width,
               y = i / width;

        obmp[i] = transpColor;

        x -= origin_x;
        y -= origin_y;

        to_logpolar(&x, &y, x, y);

        for (j = repeat_min; j <= repeat_max; j++) {
            double x1 = x, y1 = y;
            x1 += period*j;
            rotate(&x1, &y1, x1, y1);
            to_cartes(&x1, &y1, x1, y1);

            x1 += origin_x;
            y1 += origin_y;

            if (0 <= x1 && x1 < width &&
                0 <= y1 && y1 < height)
            {
                k = (int)x1 + (int)y1 * width;
                if (!pixel_eq(ibmp[k], transpColor)) {
                    obmp[i] = ibmp[k];
                    break;
                }
            }
        }
    }

}

void transform_logpolar(Pixel *obmp, Pixel *ibmp, int do_rotate, int do_repeat) {
    int i;
    int repeat_min = -5, repeat_max = 4;

    if (!do_repeat)
        repeat_min = repeat_max = 0;

    init_transform(ibmp);

    for (i = 0; i < width*height; i++) {
        int j, k;
        double x = i % width,
               y = i / width;

        obmp[i] = transpColor;

        x -= origin_x;
        y -= origin_y;

        /* display the image in log-polar coordinates */
        x *= log(r2) / width;
        y *= two_pi / height;

        for (j = repeat_min; j <= repeat_max; j++) {
            double x1 = x, y1 = y;
            x1 += period*j;
            if (do_rotate)
                rotate(&x1, &y1, x1, y1);
            to_cartes(&x1, &y1, x1, y1);

            x1 += origin_x;
            y1 += origin_y;

            if (0 <= x1 && x1 < width &&
                0 <= y1 && y1 < height)
            {
                k = (int)x1 + (int)y1 * width;
                if (!pixel_eq(ibmp[k], transpColor)) {
                    obmp[i] = ibmp[k];
                    break;
                }
            }
        }
    }

}
