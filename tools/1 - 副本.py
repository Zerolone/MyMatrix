'''
python读取8*8像素图片1.bmp， 读取每一个像素点，包括颜色，存为常量数组， 然后生成一个arduino Adafruit_Neomatrix的函数显示， 用drawpixel来显示之前的常量数组

'''
from PIL import Image

def generate_display_function(image_path, output_file, start_x=0, start_y=0):
    # 打开图片文件
    image = Image.open(image_path)

    # 调整图片大小为8x8像素
    image = image.resize((8, 8))

    # 获取图片像素数据
    pixels = list(image.getdata())

    # 生成颜色常量数组
    color_constants = "const uint16_t PROGMEM image[64] = {\n"
    for i, pixel in enumerate(pixels):
        r, g, b = pixel
        color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        color_constants += "  0x{:04X}".format(color)
        if i < 63:
            color_constants += ","
        if (i + 1) % 8 == 0:
            color_constants += "\n"
    color_constants += "};\n"

    # 生成Arduino Adafruit_NeoMatrix库的显示函数
    arduino_code = color_constants + "\n"
    arduino_code += "void displayImage(const uint16_t *image, int start_x={}, int start_y={}) {{\n".format(start_x, start_y)
    arduino_code += "  for (int y = 0; y < 8; y++) {\n"
    arduino_code += "    for (int x = 0; x < 8; x++) {\n"
    arduino_code += "      int pixel_index = y * 8 + x;\n"
    arduino_code += "      uint16_t color = pgm_read_word_near(image + pixel_index);\n"
    arduino_code += "      matrix.drawPixel(x + start_x, y + start_y, color);\n"
    arduino_code += "    }\n"
    arduino_code += "  }\n"
    arduino_code += "  matrix.show();\n"
    arduino_code += "}\n"

    # 将生成的Arduino代码写入文件
    with open(output_file, "w") as file:
        file.write(arduino_code)

    print("Arduino代码已生成并保存到文件：", output_file)

# 调用函数生成显示函数，指定起始位置为 (2, 2)
generate_display_function("bilibili1.bmp", "bilibili1.ino", start_x=2, start_y=2)
generate_display_function("bilibili2.bmp", "bilibili2.ino", start_x=2, start_y=2)

