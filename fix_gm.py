import sys

filepath = r"c:\Users\26803\source\repos\hmy-AI-bite\RPG_Project\RPG_Project\GameManager.cpp"

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the '*/' that closes the block comment
marker = '*/\n#include "GameManager.h"'
idx = content.find(marker)
if idx == -1:
    # Try with \r\n
    marker = '*/\r\n#include "GameManager.h"'
    idx = content.find(marker)
    
if idx == -1:
    print("ERROR: Could not find closing */")
    sys.exit(1)

# Get everything after */
new_content = content[idx+3:]  # skip '*/'
with open(filepath, 'w', encoding='utf-8') as f:
    f.write(new_content)
print("Done!")
print("First 300 chars:")
print(repr(new_content[:300]))
