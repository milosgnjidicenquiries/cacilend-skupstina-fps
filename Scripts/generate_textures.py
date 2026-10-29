from pathlib import Path
import numpy as np
from PIL import Image, ImageFilter, ImageDraw

OUT = Path(__file__).resolve().parents[1] / 'SourceArt' / 'Textures'
OUT.mkdir(parents=True, exist_ok=True)
SIZE = 512
SEED = 58421
rng = np.random.default_rng(SEED)

PALETTES = {
    'Asphalt': ((48, 50, 49), (93, 93, 87), 0.88),
    'Concrete': ((108, 107, 99), (176, 172, 159), 0.82),
    'Limestone': ((145, 131, 106), (221, 207, 176), 0.68),
    'Canvas': ((142, 134, 114), (220, 212, 190), 0.92),
    'Grass': ((30, 45, 24), (91, 104, 58), 0.97),
    'Wood': ((55, 32, 17), (141, 94, 48), 0.60),
    'Metal': ((40, 44, 42), (126, 127, 117), 0.48),
    'Glass': ((17, 28, 34), (77, 98, 104), 0.18),
    'InteriorWood': ((35, 18, 10), (107, 59, 29), 0.44),
    'Carpet': ((49, 7, 11), (126, 28, 32), 0.91),
    'DarkFabric': ((26, 34, 24), (92, 100, 73), 0.90),
}


def low_noise(scale=32):
    small = rng.normal(0, 1, (max(2, SIZE // scale), max(2, SIZE // scale))).astype(np.float32)
    im = Image.fromarray(np.uint8(np.clip((small + 3) * 35, 0, 255))).resize((SIZE, SIZE), Image.Resampling.BICUBIC)
    arr = np.asarray(im).astype(np.float32) / 35.0 - 3.0
    return (arr - arr.mean()) / (arr.std() + 1e-5)


y, x = np.mgrid[0:SIZE, 0:SIZE]
for name, (low, high, roughness) in PALETTES.items():
    coarse = low_noise(24)
    mid = low_noise(7)
    fine = rng.normal(0, 1, (SIZE, SIZE)).astype(np.float32)
    extra = np.zeros((SIZE, SIZE), dtype=np.float32)
    if name in ('Wood', 'InteriorWood'):
        grain = np.sin(y * (0.11 if name == 'Wood' else 0.08) + coarse * 1.8) * 0.65
        lines = np.sin(y * 0.48 + mid * 2.0) * 0.22
        extra = grain * 1.45 + lines * 0.7 + (x / SIZE - .5) * .08
    elif name in ('Canvas', 'DarkFabric', 'Carpet'):
        weave = ((np.sin(x * 0.92) > 0).astype(np.float32) - .5) * .32 + ((np.sin(y * 0.92) > 0).astype(np.float32) - .5) * .28
        extra = weave
    elif name == 'Grass':
        extra = np.sin(x * 0.16 + y * 0.42 + fine * .4) * .22
    elif name == 'Metal':
        extra = np.sin((x + y) * 0.8) * 0.06
    elif name == 'Asphalt':
        extra = np.sin(x * .17 + coarse * 2) * .08
    elif name == 'Limestone':
        extra = np.sin(y * 0.03 + coarse * 1.5) * .12

    if name in ('Wood', 'InteriorWood'):
        blend = np.clip(0.5 + coarse * 0.055 + mid * 0.018 + fine * 0.008 + extra * 0.24, 0, 1)
    else:
        blend = np.clip(0.5 + coarse * 0.12 + mid * 0.045 + fine * 0.02 + extra * 0.15, 0, 1)
    lo = np.array(low, dtype=np.float32)[None, None, :]
    hi = np.array(high, dtype=np.float32)[None, None, :]
    rgb = lo + (hi - lo) * blend[..., None]
    if name in ('Asphalt', 'Concrete', 'Limestone'):
        speck = rng.random((SIZE, SIZE))
        rgb[speck < 0.018] *= 0.73
        rgb[(speck > 0.985)] = np.minimum(255, rgb[(speck > 0.985)] + 18)
    if name == 'Glass':
        rgb += (4.0 * np.sin(x * 0.03 + coarse * 2.0))[..., None]
    if name == 'Metal':
        scratch = rng.random((SIZE, SIZE)) > 0.997
        rgb[scratch] *= 0.65
    if name == 'Asphalt':
        crack_layer = Image.new('L', (SIZE, SIZE), 0)
        draw = ImageDraw.Draw(crack_layer)
        for _ in range(15):
            px, py = int(rng.integers(0, SIZE)), int(rng.integers(0, SIZE))
            pts = [(px, py)]
            for _step in range(int(rng.integers(4, 12))):
                px += int(rng.integers(-15, 16))
                py += int(rng.integers(8, 25))
                pts.append((max(0, min(SIZE-1, px)), max(0, min(SIZE-1, py))))
            draw.line(pts, fill=180, width=1)
        crack = np.asarray(crack_layer).astype(np.float32) / 255.0
        rgb *= (1 - crack[..., None] * 0.38)
    Image.fromarray(np.uint8(np.clip(rgb, 0, 255)), 'RGB').save(OUT / f'T_{name}_BaseColor.png', optimize=True)

    height = coarse * 0.55 + mid * 0.22 + fine * 0.11 + extra * 0.45
    if name in ('Canvas', 'DarkFabric', 'Carpet'):
        height += (((np.sin(x * .92) + np.sin(y * .92)) * .5) * .22)
    if name in ('Wood', 'InteriorWood'):
        height += np.sin(y * .12 + coarse * 1.4) * .25
    gy, gx = np.gradient(height)
    strength = 1.8 if name not in ('Glass', 'Limestone') else 0.9
    nx, ny, nz = -gx * strength, -gy * strength, np.ones_like(gx)
    norm = np.sqrt(nx*nx + ny*ny + nz*nz)
    normal = np.stack(((nx/norm + 1)*127.5, (ny/norm + 1)*127.5, (nz/norm + 1)*127.5), axis=-1)
    Image.fromarray(np.uint8(np.clip(normal, 0, 255)), 'RGB').save(OUT / f'T_{name}_Normal.png', optimize=True)
    rnoise = rng.normal(0, 5, (SIZE, SIZE)) + coarse * 4 + mid * 2
    rough = np.uint8(np.clip(roughness * 255 + rnoise, 0, 255))
    Image.fromarray(rough, 'L').save(OUT / f'T_{name}_Roughness.png', optimize=True)

print(f'Wrote {len(PALETTES) * 3} PBR texture maps to {OUT}')
