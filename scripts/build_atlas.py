import os
from math import ceil
from PIL import Image

INPUT_DIR = "MafiaBar"   # directory containing banner*.png
OUTPUT_DIR = "MafiaBar"        # save in repo root (or "public" if you use GitHub Pages)
POSTER_SIZE = (512, 1024)
ATLAS_SIZE = (2048, 2048)

# Find all banner*.png files
files = [f for f in os.listdir(INPUT_DIR) if f.lower().startswith("banner") and f.lower().endswith(".png")]
files.sort()

posters_per_row = ATLAS_SIZE[0] // POSTER_SIZE[0]  # 4
posters_per_col = ATLAS_SIZE[1] // POSTER_SIZE[1]  # 2
posters_per_atlas = posters_per_row * posters_per_col  # 8

for atlas_index in range(ceil(len(files) / posters_per_atlas)):
    atlas = Image.new("RGBA", ATLAS_SIZE, (0, 0, 0, 0))
    subset = files[atlas_index * posters_per_atlas : (atlas_index + 1) * posters_per_atlas]

    for i, filename in enumerate(subset):
        img = Image.open(os.path.join(INPUT_DIR, filename)).convert("RGBA")
        img = img.resize(POSTER_SIZE, Image.LANCZOS)

        x = (i % posters_per_row) * POSTER_SIZE[0]
        y = (i // posters_per_row) * POSTER_SIZE[1]
        atlas.paste(img, (x, y))

    out_path = os.path.join(OUTPUT_DIR, f"Atlas{atlas_index + 1}.png")
    atlas.save(out_path)
    print(f"Saved {out_path}")
