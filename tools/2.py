'''
用python将gif图片每一帧都保存为一张bmp
加一个参数，是否翻转图片保存
执行的命令行获取文件名， 保存的图片以获取的文件名为前缀
从命令行获取的第二个参数可有可无


python 1.py 1.gif
python 1.py 1.gif 1
'''

import sys
import os
from PIL import Image

def extract_frames(gif_path, flip=False):
    # Open the GIF file
    gif = Image.open(gif_path)
    
    # Get the filename without extension
    filename = os.path.splitext(os.path.basename(gif_path))[0]
    
    # Iterate over each frame in the GIF
    for frame_index in range(gif.n_frames):
        # Select the current frame
        gif.seek(frame_index)
        # Convert the frame to RGBA mode
        frame = gif.convert("RGBA")
        
        # Create a new Image object with RGBA mode
        new_image = Image.new("RGBA", gif.size)
        # Composite the current frame onto the new image
        new_image.paste(frame, (0, 0), frame)
        
        # Flip the image if flip parameter is True
        if flip:
            new_image = new_image.transpose(Image.FLIP_LEFT_RIGHT)
        
        # Save the current frame as BMP with filename prefix
        new_image.save(f"{filename}_{frame_index}.bmp")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <gif_path> [flip]")
        sys.exit(1)
    
    gif_path = sys.argv[1]  # Get the gif_path from command line argument
    
    # Check if flip parameter is provided
    flip = False
    if len(sys.argv) == 3 and sys.argv[2].lower() == "1":
        flip = True
    
    extract_frames(gif_path, flip=flip)  # Set flip parameter to True if you want to flip the images, False otherwise
