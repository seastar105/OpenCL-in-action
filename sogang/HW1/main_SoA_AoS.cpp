//
//  main_SoA_AoS.cpp
//
//  Written for CSEG437_CSE5437
//  Department of Computer Science and Engineering
//  Copyright © 2021 Sogang University. All rights reserved.
//
#pragma warning(disable: 4996)
#pragma warning(disable: 6386)
#define error_check_exit if (CHECK_ERROR_CODE(ocl_stuff.err)) exit(EXIT_FAILURE);

#include "Context_SoA_AoS.h"
#include "Config_SoA_AoS.h"

Context context;
ocl_config ocl_stuff;

std::chrono::high_resolution_clock::time_point _start, _freq, _end;
float compute_time;

void create_context(void) {
    ocl_stuff.err = clGetPlatformIDs(1, &ocl_stuff.platform, NULL);
    error_check_exit
    ocl_stuff.err = clGetDeviceIDs(ocl_stuff.platform, CL_DEVICE_TYPE_GPU, 1, &ocl_stuff.device, NULL);
    error_check_exit
    ocl_stuff.context = clCreateContext(NULL, 1, &ocl_stuff.device, NULL, NULL, &ocl_stuff.err);
    error_check_exit
    ocl_stuff.queue = clCreateCommandQueue(ocl_stuff.context, ocl_stuff.device, CL_QUEUE_PROFILING_ENABLE, &ocl_stuff.err);
    error_check_exit
}

void create_program(const char *filename) {
    char *buf;
    size_t length = read_kernel_from_file(filename, &buf);
    ocl_stuff.program = clCreateProgramWithSource(ocl_stuff.context, 1, (const char**)&buf, &length, &ocl_stuff.err);
    error_check_exit
    const char options[] = "-cl-std=CL1.2";
    ocl_stuff.err = clBuildProgram(ocl_stuff.program, 1, &ocl_stuff.device, options, NULL, NULL);
//    print_build_log(ocl_stuff.program, ocl_stuff.device, "SoA_SO_GPU");
    error_check_exit
}

int main(int argc, char* argv[]) {
    char program_name[] = "Sogang CSEG475_5475 SoA_versus_AoS_20151604";
    fprintf(stdout, "\n###  %s\n\n", program_name);
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n");
    fprintf(stdout, "### INPUT FILE NAME = \t\t%s\n", INPUT_FILE_NAME);
    fprintf(stdout, "### OUTPUT FILE NAME = \t\t%s\n\n", OUTPUT_FILE_NAME);

    fprintf(stdout, "### IMAGE OPERATION = ");
    switch (IMAGE_OPERATION) {
        case SoA_GS_CPU:
            fprintf(stdout, "Structure of Arrays (GRAYSCALE) on CPU\n");
            break;
        case SoA_SO_CPU:
            fprintf(stdout, "Structure of Arrays (SOBEL) on CPU\n");
            break;
        case AoS_GS_CPU:
            fprintf(stdout, "Arrays of Structures (GRAYSCALE) on CPU\n");
            break;
        case AoS_SO_CPU:
            fprintf(stdout, "Arrays of Structures (SOBEL) on CPU\n");
            break;
        case SoA_GS_GPU:
            fprintf(stdout, "Structure of Arrays (GRAYSCALE) on GPU\n");
            break;
        case AoS_GS_GPU:
            fprintf(stdout, "Arrays of Structures (GRAYSCALE) on GPU\n");
            break;
        case SoA_SO_GPU:
            fprintf(stdout, "Structure of Arrays (SOBEL) on GPU\n");
            break;
        case AoS_SO_GPU:
            fprintf(stdout, "Arrays of Structures (SOBEL) on GPU\n");
            break;
        default:
            fprintf(stderr, "*** Error: unknown image operation...\n");
            exit(EXIT_FAILURE);
    }
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");

    read_input_image_from_file32(INPUT_FILE_NAME);   
    prepare_output_image();

    if (IMAGE_OPERATION == SoA_GS_CPU) {
        prepare_SoA_input_and_output();
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_CPU;++i) convert_to_greyscale_image_SoA_CPU();  // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_SoA_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
    }
    else if (IMAGE_OPERATION == AoS_GS_CPU) {
        prepare_AoS_input_and_output();
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_CPU;++i) convert_to_greyscale_image_AoS_CPU();  // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_output);
    }
    else if (IMAGE_OPERATION == SoA_SO_CPU) {
        prepare_SoA_input_and_output();
        convert_to_greyscale_image_SoA_CPU();
        prepare_sobel();
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_CPU;++i) sobel_SoA_CPU();
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_SoA_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
        free(context.grad);
    }
    else if (IMAGE_OPERATION == AoS_SO_CPU) {
        prepare_AoS_input_and_output();
        convert_to_greyscale_image_AoS_CPU();
        prepare_sobel();
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_CPU;++i) sobel_AoS_CPU();
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_output);
        free(context.grad);
    }
    else if (IMAGE_OPERATION == SoA_GS_GPU) {
        create_context();
        create_program(CL_FILE_NAME);
        prepare_SoA_input_and_output();
        size_t global_work_size = context.image_height * context.image_width;
        ocl_stuff.input_image = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                               sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit
        ocl_stuff.output_image = clCreateBuffer(ocl_stuff.context, CL_MEM_WRITE_ONLY,
                                                sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.kernel = clCreateKernel(ocl_stuff.program, KERNEL_NAME, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.global_work_size[0] = context.image_width;
        ocl_stuff.global_work_size[1] = context.image_height;
        ocl_stuff.local_work_size[0] = WG_WIDTH;
        ocl_stuff.local_work_size[1] = WG_HEIGHT;
        ocl_stuff.work_dim = 2;

        fprintf(stdout, "*** Data Transfer to GPU\n");

        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 0, sizeof(BYTE) * global_work_size,
                                             context.SoA_image_input.R_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, sizeof(BYTE) * global_work_size
                                             , sizeof(BYTE) * global_work_size, context.SoA_image_input.G_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 2 * sizeof(BYTE) * global_work_size
                                             , sizeof(BYTE) * global_work_size, context.SoA_image_input.B_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 3 * sizeof(BYTE) * global_work_size
                                             , sizeof(BYTE) * global_work_size, context.SoA_image_input.A_plane, 0, NULL, NULL);
        error_check_exit
        clFinish(ocl_stuff.queue);  // wait for data transfer

        ocl_stuff.err |= clSetKernelArg(ocl_stuff.kernel, 0, sizeof(cl_mem), &ocl_stuff.input_image);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.kernel, 1, sizeof(cl_mem), &ocl_stuff.output_image);
        error_check_exit
        util_reset_event_time();
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_GPU;++i) {
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            error_check_exit
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
        }
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        util_print_accumulated_device_time_1_2(N_EXECUTIONS_GPU);

        // GPU to CPU Memory

        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 0, sizeof(BYTE) * global_work_size,
                                            context.SoA_image_output.R_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, sizeof(BYTE) * global_work_size,
                                             sizeof(BYTE) * global_work_size, context.SoA_image_output.G_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 2 * sizeof(BYTE) * global_work_size,
                                             sizeof(BYTE) * global_work_size, context.SoA_image_output.B_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 3 * sizeof(BYTE) * global_work_size
                , sizeof(BYTE) * global_work_size, context.SoA_image_output.A_plane, 0, NULL, NULL);
        error_check_exit
        clFinish(ocl_stuff.queue);

        convert_SoA_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
    }
    else if (IMAGE_OPERATION == AoS_GS_GPU) {
        create_context();
        create_program(CL_FILE_NAME);
        prepare_AoS_input_and_output();
        size_t global_work_size = context.image_height * context.image_width;
        ocl_stuff.input_image = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                               sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit
        ocl_stuff.output_image = clCreateBuffer(ocl_stuff.context, CL_MEM_WRITE_ONLY,
                                                sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.global_work_size[0] = context.image_width;
        ocl_stuff.global_work_size[1] = context.image_height;
        ocl_stuff.local_work_size[0] = WG_WIDTH;
        ocl_stuff.local_work_size[1] = WG_HEIGHT;
        ocl_stuff.work_dim = 2;

        ocl_stuff.kernel = clCreateKernel(ocl_stuff.program, KERNEL_NAME, &ocl_stuff.err);
        error_check_exit
        fprintf(stdout, "*** Data Transfer to GPU\n");
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_TRUE, 0, context.image_data_bytes,
                                             context.AoS_image_input, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.kernel, 0, sizeof(cl_mem), &ocl_stuff.input_image);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.kernel, 1, sizeof(cl_mem), &ocl_stuff.output_image);
        error_check_exit
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_GPU;++i) {
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            if (CHECK_ERROR_CODE(ocl_stuff.err)) exit(EXIT_FAILURE);
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
        }
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        util_print_accumulated_device_time_1_2(N_EXECUTIONS_GPU);

        // GPU to CPU
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_TRUE, 0, context.image_data_bytes,
                                            context.AoS_image_output, 0, NULL, NULL);
        if (CHECK_ERROR_CODE(ocl_stuff.err)) exit(EXIT_FAILURE);

        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_output);
    }
    else if (IMAGE_OPERATION == SoA_SO_GPU) {
        prepare_SoA_input_and_output();
        convert_to_greyscale_image_SoA_CPU();
        prepare_sobel();

        // gpu start
        create_context();
        create_program(CL_FILE_NAME);

        // make buffer object
        size_t global_work_size = context.image_height * context.image_width;
        ocl_stuff.input_image = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                               sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit
        ocl_stuff.output_image = clCreateBuffer(ocl_stuff.context, CL_MEM_WRITE_ONLY,
                                                sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.grad = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_WRITE,
                                        sizeof(int) * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.sobel_x = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                           sizeof(int) * 9, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.sobel_y = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                           sizeof(int) * 9, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.global_work_size[0] = context.image_width;
        ocl_stuff.global_work_size[1] = context.image_height;
        ocl_stuff.local_work_size[0] = WG_WIDTH;
        ocl_stuff.local_work_size[1] = WG_HEIGHT;
        ocl_stuff.work_dim = 2;

        // create kernel
        ocl_stuff.grad_kernel = clCreateKernel(ocl_stuff.program, "SoA_SO_GPU_grad", &ocl_stuff.err);
        error_check_exit
        ocl_stuff.norm_kernel = clCreateKernel(ocl_stuff.program, "SoA_SO_GPU_norm", &ocl_stuff.err);
        error_check_exit

        // CPU to GPU
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 0, sizeof(BYTE) * global_work_size,
                                             context.SoA_image_output.R_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, sizeof(BYTE) * global_work_size
                , sizeof(BYTE) * global_work_size, context.SoA_image_output.G_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 2 * sizeof(BYTE) * global_work_size
                , sizeof(BYTE) * global_work_size, context.SoA_image_output.B_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 3 * sizeof(BYTE) * global_work_size
                , sizeof(BYTE) * global_work_size, context.SoA_image_output.A_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.sobel_x, CL_FALSE, 0, sizeof(int) * 9, sobel_x, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.sobel_y, CL_FALSE, 0, sizeof(int) * 9, sobel_y, 0, NULL, NULL);
        error_check_exit
        clFinish(ocl_stuff.queue);  // wait for data transfer

        // grad_kernel argument set
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 0, sizeof(cl_mem), &ocl_stuff.input_image);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 1, sizeof(cl_mem), &ocl_stuff.grad);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 2, sizeof(cl_mem), &ocl_stuff.sobel_x);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 3, sizeof(cl_mem), &ocl_stuff.sobel_y);
        error_check_exit

        ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 0, sizeof(cl_mem), &ocl_stuff.grad);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 1, sizeof(cl_mem), &ocl_stuff.output_image);
        error_check_exit
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_GPU;++i) {
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.grad_kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            error_check_exit
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
            // read from GPU to get min max grad
            ocl_stuff.err |= clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.grad, CL_TRUE, 0, sizeof(int)*global_work_size, context.grad, 0, NULL, &ocl_stuff.event_for_timing);
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
            float min_grad = 1e9;
            float max_grad = 0;
            for(int j=0;j<global_work_size;++j) {
                min_grad = min_grad > context.grad[j]? context.grad[j] : min_grad;
                max_grad = max_grad < context.grad[j]? context.grad[j] : max_grad;
            }
            min_grad = sqrtf(min_grad);
            max_grad = sqrtf(max_grad);
            // set norm_kernel argument
            ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 2, sizeof(float), &min_grad);
            ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 3, sizeof(float), &max_grad);
            error_check_exit
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.norm_kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
        }
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        util_print_accumulated_device_time_1_2(N_EXECUTIONS_GPU);
        // GPU to CPU Memory

        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 0, sizeof(BYTE) * global_work_size,
                                            context.SoA_image_output.R_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, sizeof(BYTE) * global_work_size,
                                            sizeof(BYTE) * global_work_size, context.SoA_image_output.G_plane, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 2 * sizeof(BYTE) * global_work_size,
                                            sizeof(BYTE) * global_work_size, context.SoA_image_output.B_plane, 0, NULL, NULL);
        error_check_exit

        clFinish(ocl_stuff.queue);
        convert_SoA_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
        free(context.grad);
    }
    else if (IMAGE_OPERATION == AoS_SO_GPU) {
        prepare_AoS_input_and_output();
        convert_to_greyscale_image_AoS_CPU();
        prepare_sobel();

        // gpu start
        create_context();
        create_program(CL_FILE_NAME);

        // make buffer object
        size_t global_work_size = context.image_height * context.image_width;
        ocl_stuff.input_image = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                               sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit
        ocl_stuff.output_image = clCreateBuffer(ocl_stuff.context, CL_MEM_WRITE_ONLY,
                                                sizeof(BYTE) * 4 * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.grad = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_WRITE,
                                        sizeof(int) * global_work_size, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.sobel_x = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                           sizeof(int) * 9, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.sobel_y = clCreateBuffer(ocl_stuff.context, CL_MEM_READ_ONLY,
                                           sizeof(int) * 9, NULL, &ocl_stuff.err);
        error_check_exit

        ocl_stuff.global_work_size[0] = context.image_width;
        ocl_stuff.global_work_size[1] = context.image_height;
        ocl_stuff.local_work_size[0] = WG_WIDTH;
        ocl_stuff.local_work_size[1] = WG_HEIGHT;
        ocl_stuff.work_dim = 2;

        // create kernel
        ocl_stuff.grad_kernel = clCreateKernel(ocl_stuff.program, "AoS_SO_GPU_grad", &ocl_stuff.err);
        error_check_exit
        ocl_stuff.norm_kernel = clCreateKernel(ocl_stuff.program, "AoS_SO_GPU_norm", &ocl_stuff.err);
        error_check_exit

        fprintf(stdout, "*** Data Transfer to GPU\n");
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.input_image, CL_FALSE, 0, context.image_data_bytes,
                                             context.AoS_image_output, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_FALSE, 0, context.image_data_bytes,
                                             context.AoS_image_output, 0, NULL, NULL);
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.sobel_x, CL_FALSE, 0, sizeof(int) * 9, sobel_x, 0, NULL, NULL);
        error_check_exit
        ocl_stuff.err = clEnqueueWriteBuffer(ocl_stuff.queue, ocl_stuff.sobel_y, CL_FALSE, 0, sizeof(int) * 9, sobel_y, 0, NULL, NULL);
        error_check_exit
        clFinish(ocl_stuff.queue);  // wait for data transfer

        // grad_kernel argument set
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 0, sizeof(cl_mem), &ocl_stuff.input_image);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 1, sizeof(cl_mem), &ocl_stuff.grad);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 2, sizeof(cl_mem), &ocl_stuff.sobel_x);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.grad_kernel, 3, sizeof(cl_mem), &ocl_stuff.sobel_y);
        error_check_exit

        // norm_kernel argument set
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 0, sizeof(cl_mem), &ocl_stuff.grad);
        ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 1, sizeof(cl_mem), &ocl_stuff.output_image);
        error_check_exit
        CHECK_TIME_START(_start, _freq);
        for(int i=0;i<N_EXECUTIONS_GPU;++i) {
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.grad_kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            error_check_exit
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
            // read from GPU to get min max grad
            ocl_stuff.err |= clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.grad, CL_FALSE, 0, sizeof(int)*global_work_size, context.grad, 0, NULL, &ocl_stuff.event_for_timing);
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
            float min_grad = 1e9;
            float max_grad = 0;
            for(int j=0;j<global_work_size;++j) {
                min_grad = min_grad > context.grad[j]? context.grad[j] : min_grad;
                max_grad = max_grad < context.grad[j]? context.grad[j] : max_grad;
            }
            min_grad = sqrtf(min_grad);
            max_grad = sqrtf(max_grad);
            // set norm_kernel argument
            ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 2, sizeof(float), &min_grad);
            ocl_stuff.err |= clSetKernelArg(ocl_stuff.norm_kernel, 3, sizeof(float), &max_grad);
            error_check_exit
            ocl_stuff.err = clEnqueueNDRangeKernel(ocl_stuff.queue, ocl_stuff.norm_kernel, ocl_stuff.work_dim, NULL, ocl_stuff.global_work_size,
                                                   ocl_stuff.local_work_size, 0, NULL, &ocl_stuff.event_for_timing);
            clWaitForEvents(1, &ocl_stuff.event_for_timing);
            util_accumulate_event_times_1_2(ocl_stuff.event_for_timing);
        }
        CHECK_TIME_END(_start, _end, _freq, compute_time);

        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        util_print_accumulated_device_time_1_2(N_EXECUTIONS_GPU);

        // GPU to CPU
        ocl_stuff.err = clEnqueueReadBuffer(ocl_stuff.queue, ocl_stuff.output_image, CL_TRUE, 0, context.image_data_bytes,
                                            context.AoS_image_output, 0, NULL, NULL);
        if (CHECK_ERROR_CODE(ocl_stuff.err)) exit(EXIT_FAILURE);
        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_output);
        free(context.grad);
    }
    else {
        fprintf(stdout, "^^^ Nothing has been done!\n");
    }
    return 0;
}

