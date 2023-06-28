import qrcode
import cv2
import pandas as pd
import numpy as np

#Generate a QR Code
qr = qrcode.QRCode(
    version=5,
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=10,
    border=4,
)

qr.add_data('https://myq.com/qr?id=uuuuuuuuuuuuuuuuuuuuuuuu')
qr.make(fit=True)

img = qr.make_image(fill_color="black", back_color="white")
img.save("qr-v5.png")

#Decode QR Code and export Excel File with binary data
imgFile = cv2.imread("qr-v5.png")

detector = cv2.QRCodeDetector()

data, verticies_array, binary_qrcode = detector.detectAndDecode(imgFile)

if(verticies_array is not None):
    print("QR Code Data:")
    print(data)
    #print(binary_qrcode)
else:
    print("ERROR")

df = pd.DataFrame(binary_qrcode).T
df = df.replace(0, 1)
df = df.replace(255, 0)
df.to_excel(excel_writer="C:/Users/mlee3/Documents/qr_v5.xlsx")

processed_data = df.values.T.tolist()
#print(processed_data)

#num_cols = len(processed_data)
#num_rows = len(processed_data[0])
num_cols = 45
num_rows = 45

v7_processed_data = np.zeros((45, 45), dtype=int).tolist()
#print(v7_processed_data)
#print(len(processed_data))
#print(len(processed_data[0]))

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
                #print([i, j])
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
#print(result_string[0:(len(result_string) - 2)])

with open("qr_v5.txt", "w") as text_file:
    text_file.write(result_string)