'''
python读取8*8像素图片1.bmp， 读取每一个像素点，包括颜色，存为常量数组， 然后生成一个arduino Adafruit_Neomatrix的函数显示， 用drawpixel来显示之前的常量数组

'''
from PIL import Image
import os


def generate_display_function(image_path, output_file, start_x=0, start_y=0):
    # 打开图片文件
    image = Image.open(image_path)

    # 调整图片大小为8x8像素
    image = image.resize((8, 8))

    # 获取图片像素数据
    pixels = list(image.getdata())
    
    varName = image_path.replace(".bmp", "")

    # 生成颜色常量数组
    color_constants = "const uint16_t PROGMEM "+varName+"[64] = {"
    for i, pixel in enumerate(pixels):
        r, g, b = pixel
        color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        color_constants += "  0x{:04X}".format(color)
        if i < 63:
            color_constants += ","
        if (i + 1) % 8 == 0:
            color_constants += ""
    color_constants += "};"
    
    arduino_code = color_constants + "\n"

    # 将生成的Arduino代码写入文件
    with open(output_file, "a") as file:
        file.write(arduino_code)

    print("Arduino代码已生成并保存到文件：", output_file)


# 调用函数生成显示函数，指定起始位置为 (2, 2)
#generate_display_function("bilibili1.bmp", "bilibili1.ino", start_x=2, start_y=2)

# 获取当前目录
current_directory = os.getcwd()

# 列出当前目录中的所有文件
files = os.listdir(current_directory)

# 找到所有以.bmp结尾的文件
#bmp_files = [file for file in files if file.endswith('.bmp')]
bmp_files = [file for file in files if file.lower().endswith('.bmp')]


# 逐个读取.bmp文件
for bmp_file in bmp_files:
    # 拼接文件路径
    file_path = os.path.join(current_directory, bmp_file)
    
    # 读取图片
    image = Image.open(file_path)
    
    # 显示图片信息
    print(f"读取了图片：{bmp_file}")
    
    generate_display_function(bmp_file, "bmp.ino", start_x=0, start_y=0)
    