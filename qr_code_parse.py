#  Authors: David Bixler, Jared Tarantino, Matthew Lee 
#  Written for Chamberlain Group LCD Display QR Code Testing

# Purpose:
# This script generates the matrices for the 5 displayable QR code versions (v3, v4, v5, v6, v7) generated from the excel workbook "qr_code.xlsx." 
# The matrices are saved the matrices to a text file generated by the script. The user can specify the name of the excel spreadsheet to use by 
# including the name of the spreadhseet (without the ".xlsx") in the command line as an argument after the name of the script. 
# Otherwise, the program reads data from "qr_code.xlsx"

import pandas as pd
import sys

# This function generates a matrix list (matrices) from the matrices created from the dataframes corresponding to the relevant QR
# code versions (v3, v4, v5, v6, v7). The function appends 'MSB2LSB' to each of the entries in the matrices and appends a newline
# after 45 columns to match the structure of the array in lcd_font_4_22.c. The function takes the qr_codes dataframe as a single 
# parameter.
def generate_matrices(qr_codes):
    # Initialize matrix list and row starts/stops for each QR code version
    matrix_list = []
    row_start = {'v3': 251, 'v4': 341, 'v5': 432, 'v6': 523, 'v7': 616}
    
    # Create dataframes for each QR code version with preset row starts/stops and remove decimal values
    for version in row_start:
        hex_dec = pd.DataFrame(qr_codes.iloc[row_start[version]:row_start[version] + 10, 5:50]) 
        hex_dec = list(hex_dec.values.flatten())
        hex = list([x for x in hex_dec if not isinstance(x, int)])
        
        matrix = []                                         # Initialize empty matrix
        count = 0                                           # Initialize count to 0
        
        for hex in hex:
            matrix.append("MSB2LSB(" + hex + '), ')          # Prepend type-cast to each hexadecimal value
            count += 1                                       # Increment count
            if count % 45 == 0:                              # Append newline after 45 columns
                matrix.append('\n')
        
        # Remove comma from last entry in each matrix and remove trailing newline character before appending matrix to matrix list
        matrix[len(matrix) - 2] = matrix[len(matrix) - 2][:len(matrix[len(matrix) - 2]) - 2]
        matrix[len(matrix) - 1] = '\0'
        matrix_list.append(matrix)                           
        
    return matrix_list                                       # Return matrix list to variable 'matrices'

# This function overwrites an existing text file with the contents of the matrices or creates a new file in the parent folder of the repository if it does not exist.
# The function takes the list of matrices as a parameter.
def file_write(matrix_list):
    output = open("../qr_code_parsed.txt", 'w')
    for idx in range(len(matrix_list)):
        output.write("V" + str(idx + 3) + " MATRIX: \n")
        output.writelines(matrix_list[idx])
        
        if idx != len(matrix_list) - 1:
            output.write('\n\n\n')
    
    output.close()

def main():
    # Set excel to 'qr_code.xlsx' unless otherwise specified, read excel spreadsheet data into qr_codes, generate matrices,
    # and write output to file 'qr_code_parsed.txt'.
    excel = "qr_code.xlsx"
    if len(sys.argv) == 2:
        if (sys.argv[1][len(sys.argv[1])-4:]) == 'xlsx':            # Use full filename if included
            excel = str(sys.argv[1])
        else:
            excel = str(sys.argv[1] + '.xlsx')                      # Append 'xlsx' to file name if not included
     
    qr_codes = pd.read_excel(excel)       
    matrices = generate_matrices(qr_codes) 
    file_write(matrices)
    
if __name__ == "__main__":
    main()