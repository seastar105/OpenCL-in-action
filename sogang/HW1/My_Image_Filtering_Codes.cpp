#include "Context_SoA_AoS.h"

inline bool valid(int y, int x) {
    return 0 <= y && y < context.image_height && 0 <= x && x < context.image_width;
}

inline BYTE normalize(float min_val, float max_val, float val) {
    float ret = 255.0f * (val - min_val) / (max_val - min_val);
    return round(ret);
}

void convert_to_greyscale_image_SoA_CPU(void) {
    for (unsigned int i = 0; i < context.image_width * context.image_height; i++) {

        BYTE intensity = BYTE(0.299f * context.SoA_image_input.R_plane[i]  // R
            + 0.587f * context.SoA_image_input.G_plane[i]  // G
            + 0.114f * context.SoA_image_input.B_plane[i]);  // B
        context.SoA_image_output.R_plane[i] = intensity;
        context.SoA_image_output.G_plane[i] = intensity;
        context.SoA_image_output.B_plane[i] = intensity;
        context.SoA_image_output.A_plane[i] = context.SoA_image_input.A_plane[i];
    }
}

void sobel_SoA_CPU(void) {
	int H = context.image_height;
	int W = context.image_width;
	int sum_x, sum_y;
    int min_grad = 1e9, max_grad = -1e9;
	for (int i = 0; i < H * W; ++i) {
        int y = i / W;
        int x = i % W;
		sum_x = 0;
		sum_y = 0;
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
				if (!valid(y + j - 1, x + k - 1)) continue;
				// Assume image_input has greyscale image, R=G=B
                int idx = (y + j - 1) * W + x + k - 1;
				sum_x += context.SoA_image_output.R_plane[idx] * sobel_x[j][k];
				sum_y += context.SoA_image_output.R_plane[idx] * sobel_y[j][k];
			}
		}
        context.grad[i] = sum_x * sum_x + sum_y * sum_y;
        if (min_grad > context.grad[i]) min_grad = context.grad[i];
        if (max_grad < context.grad[i]) max_grad = context.grad[i];
	}
    float min_val = sqrt((float)min_grad), max_val = sqrt((float)max_grad);
    for (int i = 0; i < H * W; ++i) {
        float x = sqrt(context.grad[i]);
        BYTE intensity = normalize(min_val, max_val, x);
        context.SoA_image_output.R_plane[i] = intensity;
        context.SoA_image_output.G_plane[i] = intensity;
        context.SoA_image_output.B_plane[i] = intensity;
        context.SoA_image_output.A_plane[i] = context.SoA_image_input.A_plane[i];
    }
}


void convert_to_greyscale_image_AoS_CPU(void) {
    Pixel_Channels* tmp_ptr_input = context.AoS_image_input;
    Pixel_Channels* tmp_ptr_output = context.AoS_image_output;
    for (unsigned int i = 0; i < context.image_width * context.image_height; i++) {
        BYTE intensity = BYTE(0.299f * tmp_ptr_input->R // R
            + 0.587f * tmp_ptr_input->G  // G
            + 0.114f * tmp_ptr_input->B);  // B
        tmp_ptr_output->R = intensity;
        tmp_ptr_output->G = intensity;
        tmp_ptr_output->B = intensity;
        tmp_ptr_output->A = tmp_ptr_input->A;

        tmp_ptr_input++; tmp_ptr_output++;
    }
}


void sobel_AoS_CPU(void) {
    int H = context.image_height;
    int W = context.image_width;
    int sum_x, sum_y;
    int min_grad = 1e9, max_grad = -1e9;
    for (int i = 0; i < H * W; ++i) {
        int y = i / W;
        int x = i % W;
        sum_x = 0;
        sum_y = 0;
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                if (!valid(y + j - 1, x + k - 1)) continue;
                // Assume image_input has greyscale image, R=G=B
                int idx = (y + j - 1) * W + x + k - 1;
                sum_x += context.AoS_image_output[idx].R * sobel_x[j][k];
                sum_y += context.AoS_image_output[idx].R * sobel_y[j][k];
            }
        }
        context.grad[i] = sum_x * sum_x + sum_y * sum_y;
        if (min_grad > context.grad[i]) min_grad = context.grad[i];
        if (max_grad < context.grad[i]) max_grad = context.grad[i];
    }
    float min_val = sqrt((float)min_grad), max_val = sqrt((float)max_grad);
    for (int i = 0; i < H * W; ++i) {
        float x = sqrt(context.grad[i]);
        BYTE intensity = normalize(min_val, max_val, x);
        context.AoS_image_output[i].R = intensity;
        context.AoS_image_output[i].G = intensity;
        context.AoS_image_output[i].B = intensity;
        context.AoS_image_output[i].A = context.AoS_image_input[i].A;
    }
}