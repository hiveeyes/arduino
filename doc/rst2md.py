import re
import sys
import typing as t


rst_pattern = re.compile(r"^\.\. _(?P<label>.+?): (?P<url>.+?)$")
md_format = r"[{label}]: {url}"


def rst2md(instream: t.BinaryIO):
    # print(":orphan:")
    # print()
    for line in instream.readlines():
        line = line.decode("utf-8").strip()
        matches = rst_pattern.match(line)
        if matches:
            data = matches.groupdict()
            print(md_format.format(**data))


if __name__ == "__main__":
    indata = sys.stdin.buffer
    rst2md(indata)
