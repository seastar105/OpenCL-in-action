#pragma OPENCL EXTENSION cl_amd_printf : enable
__kernel void AoS_GS_GPU(const __global uchar *input_image,
                         __global uchar *output_image) {
    int y = get_global_id(1);
    int x = get_global_id(0);
    int W = get_global_size(0);
    int offset = 4 * (y * W + x);
    uchar pixel[4];
    pixel[0] = *(input_image + offset);
    pixel[1] = *(input_image + offset + 1);
    pixel[2] = *(input_image + offset + 2);
    pixel[3] = *(input_image + offset + 3);
    uchar intensity = pixel[0] * 0.299f +   // R
                      pixel[1] * 0.587f +   // G
                      pixel[2] * 0.114f;  // B
    *(output_image + offset) = intensity;
    *(output_image + offset + 1) = intensity;
    *(output_image + offset + 2) = intensity;
    *(output_image + offset + 3) = pixel[3];
}