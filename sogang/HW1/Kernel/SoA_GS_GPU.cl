#pragma OPENCL EXTENSION cl_amd_printf : enable
__kernel void SoA_GS_GPU(const __global uchar *input_image,
                         __global uchar *output_image) {
    int y = get_global_id(1);
    int x = get_global_id(0);
    int H = get_global_size(1);
    int W = get_global_size(0);
    int gws = H * W;
    int idx = y * W + x;
    // uchar R = *(input_image + idx);
    // uchar G = *(input_image + idx + gws);
    // uchar B = *(input_image + idx + gws*2);
    // uchar A = *(input_image + idx + gws*3);
    uchar intensity = *(input_image + idx) * 0.299f +   // R
                      *(input_image + idx + gws) * 0.587f +   // G
                      *(input_image + idx + 2 * gws) * 0.114f;  // B
    *(output_image + idx) = intensity;
    *(output_image + idx + gws) = intensity;
    *(output_image + idx + 2*gws) = intensity;
    *(output_image + idx + 3*gws) = *(input_image + idx + 3*gws);
}