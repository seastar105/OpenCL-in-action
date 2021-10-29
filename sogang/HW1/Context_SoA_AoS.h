//
//  Context_SoA_AoS.h
//
//  Written for CSEG437_CSE5437
//  Department of Computer Science and Engineering
//  Copyright © 2021 Sogang University. All rights reserved.
//

#ifndef __CONTEXT_SOA_AOS__
#define __CONTEXT_SOA_AOS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <FreeImage/FreeImage.h>

#include "Util/my_OpenCL_util_2_2.h"

typedef struct _Pixel_Channels {
    BYTE R, G, B, A;
} Pixel_Channels;

typedef struct _Pixel_Planes {
    BYTE *R_plane, *G_plane, *B_plane, *A_plane;
} Pixel_Planes;

typedef struct _Context {
    FREE_IMAGE_FORMAT image_format;
    unsigned int image_width, image_height, image_pitch;
    size_t image_data_bytes;

    struct {
        FIBITMAP* fi_bitmap_32;
        BYTE* image_data;
    } input;
    struct {
        FIBITMAP* fi_bitmap_32;
        BYTE* image_data;
    } output;

    Pixel_Channels *AoS_image_input, *AoS_image_output;
    Pixel_Planes SoA_image_input, SoA_image_output;
    int* grad;  // grad[i] = s_x^2 + s_y^2
} Context;

extern Context context;

const int sobel_x[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

const int sobel_y[3][3] = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1}
};

////////////////////// Image_IO.cpp /////////////////////////////////////////
void read_input_image_from_file32(const char* filename);
void prepare_output_image(void);
void write_output_image_to_file32(const char* filename);
void prepare_SoA_input_and_output(void);
void prepare_AoS_input_and_output(void);
void convert_SoA_output_to_output_image_data(void);
void convert_AoS_output_to_output_image_data(void);
void prepare_sobel(void);

///////////// My_Image_Filtering_Codes.cpp ///////////////////////////////////
void convert_to_greyscale_image_SoA_CPU(void);
void convert_to_greyscale_image_AoS_CPU(void);

void sobel_SoA_CPU(void);
void sobel_AoS_CPU(void);
void convert_to_greyscale_image_SoA_GPU(void);
#endif // __CONTEXT_SOA_AOS__