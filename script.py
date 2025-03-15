from PIL import Image, ImageDraw, ImageFont
import os

template_folder = 'templates'
os.makedirs(template_folder, exist_ok=True)

font = ImageFont.truetype("fonts\\DINMittelschriftStd.otf", 85)

image_size = (70, 100)

def create_char_image(char, image_size):
    img = Image.new('RGB', image_size, (255, 255, 255))
    draw = ImageDraw.Draw(img)

    bbox = draw.textbbox((0, 0), char, font=font)
    width, height = bbox[2] - bbox[0], bbox[3] - bbox[1]
    position = ((image_size[0] - width) // 2, (image_size[1] - height) // 2)
    draw.text(position, char, font=font, fill=(0, 0, 0))

    img.save(os.path.join(template_folder, f'{char}.png'))

characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
for char in characters:
    create_char_image(char, image_size)

print("Character templates created!")