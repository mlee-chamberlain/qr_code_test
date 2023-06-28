import qrcode
import cv2
import pandas as pd

#Generate a QR Code
qr = qrcode.QRCode(
    version=2,
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=10,
    border=4,
)

qr.add_data('https://myq.com/qr?id=uuuuuuuuuuuuuuuuuuuuuuuu')
qr.make(fit=True)

img = qr.make_image(fill_color="black", back_color="white")
img.save("qr-v2.png")

#Decode QR Code and export Excel File with binary data
imgFile = cv2.imread("qr-v2.png")

detector = cv2.QRCodeDetector()

data, verticies_array, binary_qrcode = detector.detectAndDecode(imgFile)

if(verticies_array is not None):
    print("QR Code Data:")
    print(data)
    print(binary_qrcode)
else:
    print("ERROR")

df = pd.DataFrame(binary_qrcode).T
df = df.replace(0, 1)
df = df.replace(255, 0)
df.to_excel(excel_writer="C:/Users/mlee3/Documents/qr_v2.xlsx")

'''processed_data = df.values.tolist()
print(processed_data)

num_cols = len(processed_data)
num_rows = len(processed_data[0])

row_pos = 0

result_string = ""

for i in range(0, num_cols):
    binary_string = ""
    for j in range(row_pos, row_pos + 8):
        if(row_pos >= num_rows):
            binary_string += str(0)
        else:
            binary_string += str(processed_data[i][j])
    print(binary_string)
    hex_string = hex(int(binary_string, 2))
    print(hex_string)
    row_pos += 8'''