import argparse
from PIL import Image, ImageEnhance
import os
import sys

# Change the current working directory to the script directory
os.chdir(".")

def latlon_to_pixel(lat, lon, map_width, map_height):
    x = int((lon + 180) / 360 * map_width)
    y = int((90 - lat) / 180 * map_height)
    return x, y

def overlay_image_on_global_map(global_map_path, overlay_img_path, lat, lon, overlay_size, output_path):
    global_map = Image.open(global_map_path).convert("RGBA")
    map_width, map_height = global_map.size

    # Reduce global map opacity (50% here, adjust factor as needed)
    #alpha = global_map.split()[3]
    #alpha = ImageEnhance.Brightness(alpha).enhance(0.3)
    #global_map.putalpha(alpha)

    overlay_img = Image.open(overlay_img_path).convert("RGBA")
    overlay_img = overlay_img.resize(overlay_size, Image.ANTIALIAS)

    center_x, center_y = latlon_to_pixel(lat, lon, map_width, map_height)
    top_left_x = center_x - overlay_img.width // 2
    top_left_y = center_y - overlay_img.height // 2

    global_map.paste(overlay_img, (top_left_x, top_left_y), overlay_img)

    # Convert to RGB if saving as JPEG since JPEG doesn't support alpha
    if output_path.lower().endswith((".jpg", ".jpeg")):
        global_map = global_map.convert("RGB")

    global_map.save(output_path)
    #print(f"Overlay complete. Updated map saved to {output_path}")

def main():
    parser = argparse.ArgumentParser(description="Overlay an earth tile image onto a global map using latitude and longitude with adjustable transparency.")
    parser.add_argument("img_name", help="File path of the image tile (jpg or png)")
    parser.add_argument("lat", type=float, help="Latitude of the image center")
    parser.add_argument("lon", type=float, help="Longitude of the image center")
    parser.add_argument("--overlay_width", type=int, default=150, help="Width to resize overlay image")
    parser.add_argument("--overlay_height", type=int, default=150, help="Height to resize overlay image")
    parser.add_argument("--global_map", default="/home/nos3/eclipse-workspace/mission/src/Mission.png", help="Path to the base global map image")
    parser.add_argument("--output", default="/home/nos3/eclipse-workspace/mission/src/Mission.png", help="Path to save the updated map (prefer PNG for transparency)")

    args = parser.parse_args()

    overlay_size = (args.overlay_width, args.overlay_height)
    overlay_image_on_global_map(
        args.global_map,
        args.img_name,
        args.lat,
        args.lon,
        overlay_size,
        args.output
    )

if __name__ == "__main__":
    main()

