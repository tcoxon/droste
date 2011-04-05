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

void transform(Pixel *obmp, Pixel *ibmp) {
    int i;
    transpColor = ibmp[width*height/2 + width/2];
    printf("Transparent (hex color): #%02x%02x%02x\n",
        transpColor.r, transpColor.g, transpColor.b);

    const double two_pi = 2.0 * M_PI;
    double t_yo_scale = two_pi / height;
    double r_xo_scale = sqrt(width * width + height * height) / width;
    for (i = 0; i < width*height; i++) {
        int xo = i % width,
            yo = i / width;
        double t = (yo + 0.0) * t_yo_scale,
               r = (xo + 0.0) * r_xo_scale;
               
        double xi = r*cos(t) + width/2,
               yi = r*sin(t) + height/2;

        /* Reverse transform */
        /*{
            xi -= width/2;
            yi -= height/2;
            r = sqrt(xi*xi + yi*yi);
            t = atan2(yi, xi);
            if (t < 0) t += two_pi;
            xi = r / r_xo_scale;
            yi = t / t_yo_scale;
        }*/

        if (0 <= xi && xi < width &&
            0 <= yi && yi < height)
        {
            obmp[i] = ibmp[(int)xi + ((int)yi)*width];
        } else {
            obmp[i] = transpColor;
        }
    }

}
