import sys

patterns = [
    (': buffer', ': bytes'),
    ('neo3vm::', ''),
    ('int_', 'int'),
    ('bool_', 'bool'),
    ('from numpy import float64', ''),
    ('_Shape = Tuple[int, ...]', '')
]

def main():
    with open(sys.argv[1], 'r') as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        for pattern in patterns:
            if pattern[0] in line:  # type: str
                lines[i] = line.replace(pattern[0], pattern[1])

    # add missing import
    if 'neo3vm' in sys.argv[1]:
        lines.insert(2, 'from pybiginteger import BigInteger\n')

    with open(sys.argv[1], 'w') as f:
        f.writelines(lines)

if __name__ == "__main__":
    main()
