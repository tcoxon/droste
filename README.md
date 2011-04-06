droste
======

Applies the Droste effect transformation to an image.

The Droste Effect
-----------------

The Droste effect is a kind of recursive image, first known to appear in
[M.C. Escher's Print Gallery](http://escherdroste.math.leidenuniv.nl/index.php?menu=escher&sub=orig).

The mathematical description of the Droste effect was not discovered until 2003,
by a group of mathematicians at Leiden University, led by Prof. Hendrik Lenstra.
The following web page describes how the effect is applied:

* [http://www.josleys.com/article_show.php?id=82](http://www.josleys.com/article_show.php?id=82)

Build Instructions
------------------

droste doesn't have a real build system at the moment, only a shell script that
invokes the compiler. To build, you must have sh and gcc on your path, then:

    $ ./build.sh

Usage
-----

    $ ./droste image.bmp [--eog] [--logpolar]

### Options

* `--eog` executes `eog` (Eye of Gnome) after the image has been created
* `--logpolar` creates a partially-transformed image. These options may also
be used:
 * `--logpolar-rotate`
 * `--logpolar-repeat`
 * `--logpolar-rotate-repeat`

### Description

By default, droste will perform the full Droste effect transformation. If the
input image is `image.bmp`, droste will write the output to `image-droste.bmp`.

droste supports 24-bit and 32-bit bitmap images. 32-bit bitmaps sort of work,
but colors in the output are all wrong. 24-bit is recommended.

The centre pixel defines the color of the transparent area of the image.

Start off with your original image:

![Original image](https://github.com/downloads/tcoxon/droste/wallpicture-original.jpg "Original image")

To perform the transformation, a portion of the image needs to be marked as
'transparent'. Fill out an area with a single color (one not used elsewhere in
the image) like below, and save the image as a 24-bit bitmap. It works best if the
transparent area is small in comparison to the whole image.

![Transparent image](https://github.com/downloads/tcoxon/droste/wallpicture.jpg "Transparent image")

droste will first transform the image into log-polar coordinates:

![Log-polar coordinates](https://github.com/downloads/tcoxon/droste/wallpicture-logpolar.jpg "Log-polar coordinates")

It will then repeat the image horizontally and rotate it slightly:

![Log-polar rotated and repeated](https://github.com/downloads/tcoxon/droste/wallpicture-logpolar-repeat-rotate.jpg "Log-polar rotated and repeated")

Finally, the image is transformed back into cartesian coordinates, and the Droste transformation is complete:

![Droste effect example](https://github.com/downloads/tcoxon/droste/wallpicture-droste.jpg "Droste effect example")
