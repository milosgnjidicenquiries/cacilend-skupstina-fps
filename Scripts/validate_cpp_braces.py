"""Lightweight C++ delimiter sanity check; this is not a replacement for UnrealBuildTool."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Source"
LITERAL_OR_COMMENT = re.compile(
    r"//[^\n]*|/\*.*?\*/|\"(?:\\\\.|[^\"\\\\])*\"|'(?:\\\\.|[^'\\\\])*'",
    re.DOTALL,
)
PAIRS = {")": "(", "}": "{", "]": "["}
OPEN = set(PAIRS.values())


def check_file(path):
    source = path.read_text(encoding="utf-8", errors="replace")
    clean = LITERAL_OR_COMMENT.sub(lambda match: "\n" * match.group(0).count("\n"), source)
    stack = []
    line = 1
    for char in clean:
        if char == "\n":
            line += 1
        elif char in OPEN:
            stack.append((char, line))
        elif char in PAIRS:
            if not stack or stack[-1][0] != PAIRS[char]:
                raise ValueError("{}:{} unexpected {}".format(path.relative_to(ROOT), line, char))
            stack.pop()
    if stack:
        char, opening_line = stack[-1]
        raise ValueError("{}:{} unclosed {}".format(path.relative_to(ROOT), opening_line, char))


def main():
    source_files = sorted(list(SOURCE.rglob("*.cpp")) + list(SOURCE.rglob("*.h")))
    if not source_files:
        raise SystemExit("No C++ source files were found.")
    failures = []
    for path in source_files:
        try:
            check_file(path)
        except ValueError as exc:
            failures.append(str(exc))
    if failures:
        print("\n".join(failures))
        return 1
    print("Delimiter sanity passed for {} C++ source/header files.".format(len(source_files)))
    print("Note: an actual Unreal Engine 5.8 C++ build is still required.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
