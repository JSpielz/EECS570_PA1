// 3D Ultrasound beamforming solution check for EECS 570
// Created by: Richard Sampson, Amlan Nayak, Thomas F. Wenisch
// Revision 1.0 - 11/15/16
// Revision 2.0 - 4/12/25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

int main (int argc, char **argv) {

	int size = atoi(argv[1]);

	/* validate command line parameter */
	if (argc < 2 || !(strcmp(argv[1],"16") == 0 || strcmp(argv[1],"32") == 0 || strcmp(argv[1],"64") == 0)) {
	  printf("Usage: %s {16|32|64}\n",argv[0]);
	  printf("This program compares your beamforming output against the reference solution.\n");
	  printf("Make sure beamforming_output.bin exists in the specified output directory.\n");
	  exit(-1);
	}

	int pts_r = 1560; // Radial points along scanline
	int sls_t = size; // Number of scanlines in theta
	int sls_p = size; // Number of scanlines in phi

	float test_value; // Value from test file
	float true_value; // Correct value
	float diff_value = 0; 

	int it_r; // Iterator for r
	int it_t; // Iterator for theta
	int it_p; // Iterator for phi

	FILE *ftest;
	FILE *ftrue;

	/* Hardcoded solution directory path for Great Lakes */
	char* solution_dir = "/scratch/eecs570f26s001_class_root/eecs570f26s001_class/shared_data/PA1";

	/* Construct solution file path */
	char solution_path[256];
	sprintf(solution_path, "%s/solutions/beamforming_solution_%s.bin", solution_dir, argv[1]);

	/* Test output file path (local directory) */
	char test_path[256];
	sprintf(test_path, "./outputs/beamforming_output.bin");

	/* Open files */
	ftrue = fopen(solution_path, "rb");
	if (!ftrue) {
		fprintf(stderr, "Error: Cannot open solution file %s\n", solution_path);
		exit(1);
	}

	ftest = fopen(test_path, "rb");
	if (!ftest) {
		fprintf(stderr, "Error: Cannot open test output file %s\n", test_path);
		fclose(ftrue);
		exit(1);
	}

	printf("=== EECS 570 PA1 Solution Validation ===\n");
	printf("Input size: %s scanlines\n", argv[1]);
	printf("Reference solution: %s\n", solution_path);
	printf("Test output: %s\n", test_path);
	printf("Starting validation...\n\n");

	/* Compare files element by element */
	for (it_t = 0; it_t < sls_t; it_t++) {
		for (it_p = 0; it_p < sls_p; it_p++) {
			for (it_r = 0; it_r < pts_r; it_r++) {
				if (fread(&test_value, sizeof(float), 1, ftest) != 1) {
					fprintf(stderr, "Error: Unexpected end of test file\n");
					fclose(ftest);
					fclose(ftrue);
					exit(1);
				}
				if (fread(&true_value, sizeof(float), 1, ftrue) != 1) {
					fprintf(stderr, "Error: Unexpected end of solution file\n");
					fclose(ftest);
					fclose(ftrue);
					exit(1);
				}
				diff_value += (test_value - true_value) * (test_value - true_value);
			}
		}
	}

	/* Calculate RMS error */
	diff_value = (float)sqrt(diff_value/sls_t/sls_p/pts_r);
	
	printf("Validation complete!\n");
	printf("RMS Error: %e\n", diff_value);
	
	/* Provide grading feedback */
	if (diff_value < 1e-16) {
		printf("✓ EXCELLENT! Output is correct (RMS error < 1e-16)\n");
		printf("  This meets the grading requirement for correctness.\n");
	} else {
		printf("✗ ISSUE DETECTED! Output differs beyond acceptable range (RMS error > 1e-16)\n");
		printf("  Please check your implementation.\n");
	}

	fclose(ftest);
	fclose(ftrue);

	return 0;
}
