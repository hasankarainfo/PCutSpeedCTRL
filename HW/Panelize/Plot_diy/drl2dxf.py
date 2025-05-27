from dxfwrite import DXFEngine as dxf
from sys import argv
import re

#Run command: python3 drl2dxf.py panelize.drl

if len(argv) < 2:
    argv.append("panelize.drl")
    print("Need filename")

in_filename = argv[1]
out_filename = in_filename[:-4] 

drawing = (dxf.drawing(out_filename+  ".dxf"))

tools = {}
current_tool = None
for line in open(in_filename).readlines():
    line = line.strip()
    match = re.match("^T(\d+)C(.+)$", line)
    if match:
        tool_id = int(match.group(1))
        size = float(match.group(2))
        tools[tool_id] = size

        drawing.add_layer("holes_"+ str(size)+"mm")
        continue
    match = re.match("^T(\d+)$", line)
    if match:
        current_tool = int(match.group(1))
        continue
    match = re.match("^X(.+)Y(.+)$", line)
    if match:
        x = float(match.group(1))
        y = float(match.group(2))
        size = tools[current_tool]

        drawing.add(dxf.circle(radius=size/2, center=(x, y), layer="holes_"+ str(size)+"mm" ))

print("tools:")
print(tools)

drawing.save()