// #pragma OPENCL EXTENSION cl_amd_printf : enable

__kernel void AoS_SO_GPU_grad(const __global uchar *input,
                         __global int *grad, __global int *sobel_x, __global int *sobel_y) {
    int y = get_global_id(1);
    int x = get_global_id(0);
    int H = get_global_size(1);
    int W = get_global_size(0);
    int sum_x = 0, sum_y = 0;
    int sobel_idx = -1;
    int offset;
    for(int i=-1;i<=1;++i) {
      for(int j=-1;j<=1;++j) {
        ++sobel_idx;
        if (y+i < 0 || y+i >= H) continue;
        if (x+j < 0 || x+j >= W) continue;
        offset = ((y+i) * W + x + j) * 4;
        sum_x += sobel_x[sobel_idx] * input[offset];
        sum_y += sobel_y[sobel_idx] * input[offset];
      }
    }
    grad[y*W + x] = sum_x * sum_x + sum_y * sum_y;
}

__kernel void AoS_SO_GPU_norm(__global int *grad, __global uchar *output, const float min_grad, const float max_grad) {
    int y = get_global_id(1);
    int x = get_global_id(0);
    int W = get_global_size(0);
    int idx = y * W + x;
    float val = sqrt((float)grad[idx]);
    val = 255.0f * (val - min_grad) / (max_grad - min_grad);
    uchar intensity = round(val);
    idx *= 4;
    *(output + idx) = intensity;
    *(output + idx + 1) = intensity;
    *(output + idx + 2) = intensity;
}
/*
108740 102544 113914 127394 126216 
105010   4100   4516   1882   1480 
124100   2890   3770   3626   4552 
128538    962   1768   2000    244 
116770   2434   3364   4426   3434 
*/