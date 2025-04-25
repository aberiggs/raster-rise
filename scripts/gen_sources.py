import os
import glob

SRC_DIR = 'src'
OUT_FILE = os.path.join(SRC_DIR, 'meson.build')

def main():
    sources = []
    for ext in ('*.cpp', '*.c', '*.cc'):
        sources.extend(glob.glob(f'{SRC_DIR}/**/{ext}', recursive=True))

    sources = sorted(sources)
    with open(OUT_FILE, 'w') as f:
        f.write('sources += [\n')
        for s in sources:
            f.write(f"  '{s}',\n")
        f.write(']\n')

if __name__ == '__main__':
    main()
