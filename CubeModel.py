import magiccube
import os
import time
import MoveTable

cube_size = 4
n = cube_size ** 2

solved_state = "W" * n + "R" * n + "G" * n + "O" * n + "B" * n + "Y" * n
cube = magiccube.Cube(cube_size, solved_state)

def clear_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

def scramble(goob): 
    if goob:
        cube.rotate("R' L L U D' F B' B' R' L")
        #cube.rotate("L' R B B F' D U' L' L' R")

def solve_green_face():
    




def explore_until_constraints_satisfied(sel_coord, tgt_coord):
    allmoves = ["U","F","R","L","B","D",
                "U'","F'","R'","L'","B'", "D'"
                "2U","2F","2R","2L","2B", "2D"
                "2U'","2F'","2R'","2L'","2B'", "2D'"]
    copy = cube
    while sel_coord != tgt_coord:
        for move in allmoves:
            pass

    

    

# logic flow for white face:
# 1. find location of all 4 center white pieces
# 2. note if any are already placed in the correct location (front center)
    # 3. if YES, check if any of the other NON correctly placed pieces are on the same x, y, or z axis (same coordinate)
        # if axis is the same, 


#print(cube.cube[0][3][3])
#print(cube.find_piece('WG'))
scramble(True)

try: 
    while True: 
        print(cube)
        time.sleep(0.5)
        clear_terminal()
        

except KeyboardInterrupt:
    clear_terminal()
