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

/* Transform x and y into logarithmic polar coordinates:
    where z = x+jy,
    z' = log(z)
    or  x' = log(sqrt(x*x + y*y))
        y' = atan(y/x)
 */
void to_logpolar(double *rx, double *ry, double x, double y) {
    *rx = log(sqrt(x*x + y*y));
    *ry = atan2(y, x);
}

/* Scaling factor between one iteration and the next one out.
   For the moment, just assume it's always 1.7. */
static const double r2_over_r1 = 1.7;
static const double two_pi = 2.0 * M_PI;

/* Rotate the coordinates so that diagonal coincides with the imaginary
   axis and shrunk to be 2pi high.
    Where z = x+jy, and a = atan(log(r2/r1)/2pi),
    z' = z cos(a) e**ja
    or  x' = x cos(a) cos(a) - y cos(a) sin(a)
        y' = y cos(a) cos(a) + x cos(a) sin(a)
 */
static double a, period, cosa, sina, cosacosa, cosasina;
void rotate(double *rx, double *ry, double x, double y) {
    *rx = x*cosacosa - y*cosasina;
    *ry = y*cosacosa + x*cosasina;
}

void init_transform(void) {
    period = log(r2_over_r1);
    a = atan2(period, two_pi);
    cosa = cos(a);
    sina = sin(a);
    cosacosa = cosa*cosa;
    cosasina = cosa*sina;
}

/* Transform log-polar coords x and y into cartesian coordinates.
    Where z = x+jy,
    z' = e**z
    or  x' = cos(y) e**x
        y' = sin(y) e**x */
void to_cartes(double *rx, double *ry, double x, double y) {
    double e_x = exp(x);
    *rx = cos(y) * e_x;
    *ry = sin(y) * e_x;
}

void transform(Pixel *obmp, Pixel *ibmp) {
    int i;

    init_transform();

    transpColor = ibmp[width*height/2 + width/2];
    printf("Transparent (hex color): #%02x%02x%02x\n",
        transpColor.r, transpColor.g, transpColor.b);

    for (i = 0; i < width*height; i++) {
        int j, k;
        double x = i % width,
               y = i / width;

        obmp[i] = transpColor;

        x -= width/2;
        y -= height/2;

        /* Do x and y need to be scaled here? */
        to_logpolar(&x, &y, x, y);
        rotate(&x, &y, x, y);

        for (j = -2; j < 10; j++) {
            double x1 = x, y1 = y;
            to_cartes(&x1, &y1, x1 + period*j, y1);

            x1 += width/2;
            y1 += height/2;

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
