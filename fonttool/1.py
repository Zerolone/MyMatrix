'''
pip install pillow
ChillBitmap7x.ttf
'''
from PIL import Image, ImageDraw, ImageFont

# 定义字体文件路径和大小
font_path = 'ChillBitmap7x.ttf'
font_size = 8

# 要生成的文本
text = "周"

# 创建空白的8x8图片
img_size = (8, 8)
img = Image.new('RGB', img_size, color='white')
draw = ImageDraw.Draw(img)
font = ImageFont.truetype(font_path, font_size)

# 逐个字符生成对应的小图片
for char in text:
    char_img = Image.new('RGB', img_size, color='white')
    char_draw = ImageDraw.Draw(char_img)
    char_draw.text((0, 0), char, fill='black', font=font)
    char_img.save(f'{char}.bmp')

# 示例：生成'A'字符的小图片
char_A_img = Image.new('RGB', img_size, color='white')
char_A_draw = ImageDraw.Draw(char_A_img)
char_A_draw.text((0, 0), ':', fill='black', font=font)
char_A_img.save('char_.bmp')