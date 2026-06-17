from PIL import Image
from PIL import ImageFilter

mode = "gaussian_blur"

im = Image.open(r"/home/nos3/Desktop/github-nos3/sims/build/bin/cam.bin")
#im = Image.open(r"C:\Users\louis.lolive\Downloads\Mashhad.jpg")
im.load()
#im.show()
box = (443, 220, 536, 291)
region = im.crop(box)
#region.show()

if mode == "black_quare":
    for x in range(region.width):
        for y in range(region.height):
            region.putpixel((x, y), (0,0,0))


if mode == "blur":
    region = region.filter(ImageFilter.BLUR)

if mode == "gaussian_blur":
    region = region.filter(ImageFilter.GaussianBlur(radius = 5))
#region.show()
im.paste(region, box)

#im.show()

im.save(r"/home/nos3/Desktop/github-nos3/sims/build/bin/cam.bin", 'jpeg', quality=95)
#im.save(r"C:\Users\louis.lolive\Downloads\Mashhad_censored.jpg", 'jpeg', quality=95)