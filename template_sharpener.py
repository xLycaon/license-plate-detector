import cv2
import numpy as np
import os

# Path to templates folder
template_folder = "C:\\Users\\vladm\\source\\repos\\license-plate-detector\\templates\\"

# List of template characters (or just a list of image files)
template_characters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

# Loop through each character template
for char in template_characters:
    template_path = os.path.join(template_folder, f"{char}.png")
    
    # Load the template image in grayscale
    template_image = cv2.imread(template_path, cv2.IMREAD_GRAYSCALE)
    
    # Apply Gaussian Blur to reduce noise (optional)
    blurred = cv2.GaussianBlur(template_image, (5, 5), 0)
    
    # Sharpen the image
    sharpening_kernel = np.array([[-1, -1, -1], [-1, 9, -1], [-1, -1, -1]])
    sharpened = cv2.filter2D(blurred, -1, sharpening_kernel)
    
    # Apply edge detection (Canny edge detector)
    edges = cv2.Canny(sharpened, 100, 200)
    
    # Save the sharpened and edge-detected template
    sharpened_template_path = os.path.join(template_folder, f"{char}_sharpened.png")
    cv2.imwrite(sharpened_template_path, edges)

print("Sharpened templates saved!")
