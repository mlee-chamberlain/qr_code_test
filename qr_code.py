import qrcode
import sys
import os
import cv2
import pandas as pd
import numpy as np

def main():
    # Take in URL and folder name as arguments
    checkArgs()

    url = sys.argv[1]
    parentDirName = sys.argv[2]
    try:
        parentDir = os.mkdir("../" + parentDirName)
    except OSError as error: 
        print(error)

    for curr_version in range(3, 8):
        #Make new directory to store generated files pertaining to this QR version
        currPath = "../" + parentDirName + "/qr-v" + str(curr_version)
        
        try:
            currDir = os.mkdir(currPath)
        except OSError as error: 
            print(error)

        #Generate a QR Code
        qr = qrcode.QRCode(
            version=curr_version,
            error_correction=qrcode.constants.ERROR_CORRECT_L,
            box_size=10,
            border=4,
        )

        qr.add_data(url)
        qr.make(fit=True)

        img = qr.make_image(fill_color="black", back_color="white")
        img.save(currPath + "/qr-v" + str(curr_version) + ".png")

        #Decode QR Code and export Excel File with binary data
        imgFile = cv2.imread(currPath + "/qr-v" + str(curr_version) + ".png")

        detector = cv2.QRCodeDetector()

        data, verticies_array, binary_qrcode = detector.detectAndDecode(imgFile)

        if(verticies_array is not None):
            print("QR Code Data:")
            print(data)
        else:
            print("ERROR")

        df = pd.DataFrame(binary_qrcode).T
        df = df.replace(0, 1)
        df = df.replace(255, 0)
        df.to_excel(excel_writer=currPath + "/qr-v" + str(curr_version) + ".xlsx")

        processed_data = df.values.T.tolist()

        num_cols = 45
        num_rows = 45

        v7_processed_data = np.zeros((45, 45), dtype=int).tolist()

        for i in range(0, len(processed_data)):
            for j in range(0, len(processed_data[0])):
                v7_processed_data[i][j] = processed_data[i][j]

        row_pos = 0

        result_string = ""

        count = 0

        while(row_pos < num_rows):
            for i in range(0, num_cols):
                binary_string = ""
                for j in range(row_pos, row_pos + 8):
                    if(j >= num_rows):
                        binary_string += str(0)
                    else:
                        binary_string += str(v7_processed_data[i][j])
                print(binary_string)
                hex_string = hex(int(binary_string, 2))
                print(hex_string)
                result_string += "MSB2LSB(" + hex_string + "), "
                count += 1
                if(count%45 == 0):
                    result_string += "\n"
            row_pos += 8

        result_string = result_string[0:(len(result_string) - 3)]
        print(result_string)

        with open(currPath + "/qr-v" + str(curr_version) + ".txt", "w") as text_file:
            text_file.write(result_string) 

def checkArgs():
    # Simple function for check that the user input exactly 1 arguments: the URL
    
    print(len(sys.argv))
    if(len(sys.argv) < 3): # Check if they didn't include the URL or folder name
        print("Must include URL and folder name!")
        exit(-1)
    
    elif(len(sys.argv) > 3): # Check if they included too many arguments 
        print("Too many args!")
        exit(-1)

if __name__ == "__main__":
    main()