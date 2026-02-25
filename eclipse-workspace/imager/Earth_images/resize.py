from PIL import Image
Image.MAX_IMAGE_PIXELS=1000000000
import math
import sys
from pathlib import Path

def nearest_power_of_two(n: int) -> int:
    """Return the power of 2 closest to n (e.g. 300 -> 256, 600 -> 512)."""
    if n <= 0:
        raise ValueError("Dimension must be positive")

    lower = 2 ** math.floor(math.log2(n))
    upper = 2 ** math.ceil(math.log2(n))

    # If exactly between, prefer the larger one; change rule if you want
    if n - lower < upper - n:
        return lower
    else:
        return upper

def resize_to_nearest_pow2(input_path: str, output_path: str = None) -> None:
    """Resize an image so width and height are the nearest power-of-two values."""
    img = Image.open(input_path)
    w, h = img.size

    new_w = nearest_power_of_two(w)
    new_h = nearest_power_of_two(h)

    #resized = img.resize((new_w, new_h), Image.LANCZOS)
    resized = img.resize((new_w, new_h), Image.NEAREST)

    if output_path is None:
        input_path = Path(input_path)
        output_path = str(input_path.with_name(
            f"{input_path.stem}_pow2{input_path.suffix}"
        ))

    resized.save(output_path)
    print(f"Saved resized image to: {output_path}")
    print(f"Original size: {w}x{h}, new size: {new_w}x{new_h}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python resize_pow2.py <image_path> [output_path]")
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2] if len(sys.argv) >= 3 else None
    resize_to_nearest_pow2(in_path, out_path)

