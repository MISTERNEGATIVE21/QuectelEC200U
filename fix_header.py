
lines = []
with open('c:/Users/Administrator/Documents/GitHub/QuectelEC200U/src/QuectelEC200U.h', 'r') as f:
    lines = f.readlines()

# 1-indexed lines to keep: 1-294, 495-end
# 0-indexed slicing: [0:294] + [494:]
# Line 295 is index 294. So we want up to index 294 (exclusive? no, slice is exclusive of end).
# lines[0] is line 1.
# lines[293] is line 294.
# lines[294] is line 295.
# So we want lines[:294] -> indices 0 to 293 (lines 1 to 294).

# We want to skip lines 295 to 494.
# Line 494 is index 493.
# Line 495 is index 494.
# So we want to resume at index 494 (line 495).

new_lines = lines[:294] + lines[494:]

with open('c:/Users/Administrator/Documents/GitHub/QuectelEC200U/src/QuectelEC200U.h', 'w') as f:
    f.writelines(new_lines)

print(f"Fixed header. Total lines: {len(new_lines)}")
