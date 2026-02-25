from PIL import Image, ImageDraw, ImageFont

IMG_PATH = "/home/nos3/eclipse-workspace/mission/src/Mission_start.png"
OUTPUT_PATH = "/home/nos3/eclipse-workspace/mission/src/Mission_start_target.png"
DOT_RADIUS = 8
FONT_SIZE = 54

def latlon_to_pixel(lat, lon, map_width, map_height):
    # Convert latitude and longitude to pixel coordinates using equirectangular projection
    x = int((lon + 180) / 360 * map_width)
    y = int((90 - lat) / 180 * map_height)
    return x, y

# Read locations from config.txt
locations = []
with open("/home/nos3/eclipse-workspace/mission/config/targets.txt", "r", encoding="utf-8") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) >= 3:
            lat, lon = float(parts[0]), float(parts[1])
            name = "_".join(parts[2:])
            locations.append((lat, lon, name))
            print(f"Loaded: {name} at {lat}, {lon}")

# Load base image
img = Image.open(IMG_PATH)
w, h = img.size

# Prepare to draw
draw = ImageDraw.Draw(img)
try:
    # Common Linux font path for DejaVu Sans
    font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONT_SIZE)
except IOError:
    font = ImageFont.load_default()
    print("Could not load DejaVuSans.ttf, using default font (not resizable).")

# Draw each location
for lat, lon, name in locations:
    x, y = latlon_to_pixel(lat, lon, w, h)
    draw.ellipse((x - DOT_RADIUS, y - DOT_RADIUS, x + DOT_RADIUS, y + DOT_RADIUS), fill="orange")
    draw.text((x + DOT_RADIUS + 2, y - DOT_RADIUS), name, fill="black", font=font)

# Save and display result
img.save(OUTPUT_PATH)
img.show()

