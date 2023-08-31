# offsets for the direction each pawn has to move
directionalOffsets = [-8, 8]
def GetPawnPushes(startSquare, colorToMove) :
  attacks = 0
  # adds the square in front of the pawn to the attack mask
  attacks |= 1 << (startSquare + directionalOffsets[colorToMove]);
  # if it's on the starting rank for it's color
  if(startSquare << 3 == 6 - (5 * colorToMove)):
    # add double pawn pushes, we don't need to care about the obstacles, that check is handled later
    attacks |= 1 << (startSquare + directionalOffsets[colorToMove] * 2);
  return attacks

print("{")
for i in range(8,55):
  print(str(GetPawnPushes(i, 0)) + ", ")
print("}, ")
print("{")
for i in range(8,63):
  print(str(GetPawnPushes(i, 1)) + ", ")
print("}")
