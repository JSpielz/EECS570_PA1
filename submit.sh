#!/bin/bash
#SBATCH --account=eecs570f26s001_class
#SBATCH --job-name=beamform
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=36
#SBATCH --mem=180G
#SBATCH --time=00:05:00
#SBATCH --partition=standard
#SBATCH --output=beamform_%j.out
#SBATCH --error=beamform_%j.err

# EECS 570 PA1 - Student SLURM Submission Script
# 
# Usage: sbatch submit.sh
# 
# This script runs your beamforming program on Great Lakes
# Make sure to modify the INPUT_SIZE variable below for your desired input

# ===== CONFIGURATION =====
# Change this to test different input sizes: 16, 32, or 64
INPUT_SIZE=64

# ===== JOB SETUP =====
echo "=== EECS 570 PA1 Beamforming Job ==="
echo "Job ID: $SLURM_JOB_ID"
echo "Input size: $INPUT_SIZE scanlines"
echo "Cores allocated: $SLURM_CPUS_PER_TASK"
echo "Job started at: $(date)"
echo ""

# Set up output directory
mkdir -p "$SLURM_SUBMIT_DIR/outputs"



# Compile your program
echo "Compiling beamforming program..."
gcc -mavx2 -pthread -o beamform beamform.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed. Please check your code."
    exit 1
fi
echo "✓ Compilation successful"

# Compile solution checker
echo "Compiling solution checker..."
gcc -o solution_check solution_check.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Solution checker compilation failed."
    exit 1
fi
echo "✓ Solution checker compilation successful"
echo ""

# Run the beamforming program
echo "Running beamforming with ${INPUT_SIZE}-scanline input..."
echo "Output will be written to: ./outputs"
echo ""

# Execute the program
./beamform $INPUT_SIZE

# Check results
if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Beamforming completed successfully!"
    
    # Check output file
    output_file="./outputs/beamforming_output.bin"
    if [ -f "$output_file" ]; then
        file_size=$(ls -lh "$output_file" | awk '{print $5}')
        echo "✓ Output file created: $output_file"
        echo "✓ File size: $file_size"
        
        # Validate output against reference solution
        echo ""
        echo "=== Validating Output ==="
        ./solution_check $INPUT_SIZE
        
    else
        echo "✗ Output file not found"
    fi
else
    echo ""
    echo "✗ Beamforming failed. Check the error messages above."
fi

echo ""
echo "Job completed at: $(date)"
echo "Check the output file: ./outputs/beamforming_output.bin"
