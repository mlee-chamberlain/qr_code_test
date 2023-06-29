#  Authors: David Bixler, Jared Tarantino, Matthew Lee 
#  Written for Chamberlain Group LCD Display QR Code Testing

# Purpose:
# This script generates the matrices for the 5 displayable QR code versions (v3, v4, v5, v6, v7) generated from the excel workbook "qr_code.xlsx." The matrices are
# saved the matrices to a text file generated by the script. The user can specify the name of the excel spreadsheet to use by including the name 
# of the spreadhseet (without the ".xlsx") in the command line as an argument. Otherwise, the program reads data from "qr_code.xlsx"

import pandas as pd
import sys

# This function generates a parent list of the respective matrices from each QR code version. The function takes the empty matrix list and the respective hex list
# of each version as parameters.
def generate_matrix_list(matrix_list, hex_list):
    matrix = []                                          # Create empty matrix list
    count = 0                                            # Initialize count to 0
    
    for hex in hex_list:
        matrix.append("MSB2LSB(" + hex + '), ')          # Prepend type-cast to each hexadecimal value
        count += 1                                       # Increment count
        if count % 45 == 0:                              # Append newline after 45 columns
            matrix.append('\n')
        
    matrix[len(matrix) - 1] = matrix[len(matrix) - 1][:len(matrix[len(matrix) - 1]) - 2]    # Remove ',' from last matrix entry
    matrix_list.append(matrix)                                                              # Append version matrix to matrix list
    return matrix_list                                                                      # Return matrix list

# This function overwrites an existing text file with the contents of the matrices or creates a new file if it does not exist.
# The function takes the list of matrices as a parameter.
def file_write(matrix_list):
    output = open("qr_code_parsed.txt", 'w')
    for idx in range(len(matrix_list)):
        output.write("V" + str(idx + 3) + " MATRIX: \n")
        output.writelines(matrix_list[idx])
        output.write('\n\n\n')
    
    output.close()

def main():
    # Initialize matrix list and read QR code data from spreadsheet. 
    # Then, create dataframes for each QR code version and remove decimals from each list.
    matrix_list = []
    excel = "qr_code.xlsx"
    if len(sys.argv) == 2:
        excel = str(sys.argv[1] + '.xlsx')
    
    qr_codes = pd.read_excel(excel)       

    v3 = pd.DataFrame(qr_codes.iloc[251:261, 5:50])
    v3 = list(v3.values.flatten())
    v3_hex = list([x for x in v3 if not isinstance(x, int)])

    v4 = pd.DataFrame(qr_codes.iloc[341:351, 5:50])
    v4 = list(v4.values.flatten())
    v4_hex = list([x for x in v4 if not isinstance(x, int)])

    v5 = pd.DataFrame(qr_codes.iloc[432:442, 5:50])
    v5 = list(v5.values.flatten())
    v5_hex = list([x for x in v5 if not isinstance(x, int)])

    v6 = pd.DataFrame(qr_codes.iloc[523:533, 5:50])
    v6 = list(v6.values.flatten())
    v6_hex = list([x for x in v6 if not isinstance(x, int)])

    v7 = pd.DataFrame(qr_codes.iloc[616:626, 5:50])
    v7 = list(v7.values.flatten())
    v7_hex = list([x for x in v7 if not isinstance(x, int)])

    # Generate full matrix list
    matrix_list = generate_matrix_list(matrix_list, v3_hex)
    matrix_list = generate_matrix_list(matrix_list, v4_hex)
    matrix_list = generate_matrix_list(matrix_list, v5_hex) 
    matrix_list = generate_matrix_list(matrix_list, v6_hex)
    matrix_list = generate_matrix_list(matrix_list, v7_hex) 
    
    # Write to file 
    file_write(matrix_list)
    
if __name__ == "__main__":
    main()