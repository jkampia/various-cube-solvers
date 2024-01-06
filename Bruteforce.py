# script that will generate a mock cube and use it to find bruteforce algorithms to solve the middle 2x2 of each face

import magiccube
from magiccube.cube_base import Color
import os
import pandas as pd

allmoves = ["U","F","R","L","B","D", # every possible (slice) move 
                "U'","F'","R'","L'","B'", "D'",
                "2U","2F","2R","2L","2B", "2D",
                "2U'","2F'","2R'","2L'","2B'", "2D'"]

relevant_pieces = [[0,1,1],[0,1,2],[0,2,1],[0,2,2], # left
                   [3,1,1],[3,1,2],[3,2,1],[3,2,2], # right
                   [1,1,0],[2,1,0],[1,2,0],[2,2,0], # back
                   [1,1,3],[2,1,3],[1,2,3],[2,2,3], # front
                   [1,0,1],[1,0,2],[2,0,1],[2,0,2], # bottom
                   [1,3,1],[1,3,2],[2,3,1],[2,3,2], # top
                   ]
                   
move_affects = {
    0 : {0 : ["L", "L'"], 1: ["2L", "2L'"], 2: ["2R", "2R'"], 3: ["R", "R'"]}, # x 
    1 : {0 : ["D", "D'"], 1: ["2D", "2D'"], 2: ["2U", "2U'"], 3: ["U", "U'"]}, # y
    2 : {0 : ["B", "B'"], 1: ["2B", "2B'"], 2: ["2F", "2F'"], 3: ["F", "F'"]}, # z
}

axis_dict = { # matches a x,y,z coordinate of the cube to the axis sticking out at that coord
    0 : {0 : [0], 1 : [None], 2 : [None], 3 : [0]}, # x
    1 : {0 : [1], 1 : [None], 2 : [None], 3 : [1]}, # y
    2 : {0 : [2], 1 : [None], 2 : [None], 3 : [2]}, # z
}

cube_size = 4
n = cube_size ** 2
solved_state = "G" * n + "G" * n + "G" * n + "G" * n + "G" * n + "G" * n
cube = magiccube.Cube(cube_size, solved_state)

def clear_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

def contains_only_thischar(value, label):
    if not all(char == label for char in str(value)):
        return False
    return True

def pull_face_pieces(dict, label): # label = piece you want to match
    faces = []
    for key, value in dict.items():
        if contains_only_thischar(value, label):
            faces.append(key) # only need coords (key)
    if len(faces) != 4:
        print("Incorrect # of faces returned by contains_only_thischar()")
        return
    return faces

def find_moves_that_will_affect_piece(coords):
    move_list = []
    for i in range(3):
        move_list += move_affects[i][coords[i]]
    return move_list

def get_axis(coord): 
    axis = []
    for i in range(len(coord)):
        axis += axis_dict[i][coord[i]]
    while None in axis: # delete none type elements
        axis.remove(None)
    return axis

#coord = [3,1,2]
#cube.get_piece((coord[0], coord[1], coord[2])).set_piece_color(get_axis(coord), Color.create("W"))
#movelist = find_moves_that_will_affect_piece(coord)
#print(movelist)
#print(cube)

def recursive_search(moves, path_set=[], depth=0, MAX_DEPTH=0):
    if depth > MAX_DEPTH:
        return path_set

    if depth == 0:
        new_path_set = [[move] for move in moves]
    else:
        new_path_set = [path + [move] for path in path_set for move in moves]

    # NEED TO CALCULATE NEW VALID SET OF MOVES
    

    return recursive_search(moves, new_path_set, depth + 1, MAX_DEPTH)

# we want to solve the first white piece into the top right of the front face; so, desired coordinate = [2, 2, 3]

# remove front pieces from consideration


init_maxdepth = 3
valid_movelist = recursive_search(allmoves, [], 0, init_maxdepth)
pd.DataFrame(valid_movelist).to_csv("Pregenerated_movelists.csv", index=False)


def top_right_white_face():
    front_pieces = [[1,1,3],[2,1,3],[1,2,3],[2,2,3]]
    relevant_pieces_copy = relevant_pieces
    for piece in front_pieces:
        relevant_pieces_copy.remove(piece)
    desired_location = [2,2,3] # top right of front 2x2 face
    piece = 1
    master_list = []
    for relevant_piece in relevant_pieces_copy:
        final_movelist = []
        iter = 0
        for move_list in valid_movelist:
            if iter % 100 == 0:
                clear_terminal()
                print(f'Evaluating path {iter} / {len(valid_movelist)} at max depth {init_maxdepth}. Piece # {piece}')
            iter += 1
            newcube = magiccube.Cube(cube_size, solved_state)
            newcube.get_piece((relevant_piece[0], relevant_piece[1], relevant_piece[2])).set_piece_color(get_axis(relevant_piece), Color.create("W"))
            for move in move_list:
                newcube.rotate(move)
            #final_location = [newcube.find_piece("W")[0][0], newcube.find_piece("W")[0][1], newcube.find_piece("W")[0][2]]
                #print(str(newcube.cube[desired_location[0], desired_location[1], desired_location[2]]))
                #print(newcube)
            if str(newcube.cube[desired_location[0], desired_location[1], desired_location[2]]) == "W":
                final_movelist = move_list
                break
        piece += 1 
        list_el = [relevant_piece, final_movelist]  
        master_list.append(list_el)
    print(f'Master list: {master_list} ')

def top_left_white_face():
    front_pieces = [[1,1,3],[2,1,3],[1,2,3],[2,2,3]]
    relevant_pieces_copy = relevant_pieces
    print(relevant_pieces)
    for piece in front_pieces:
        relevant_pieces_copy.remove(piece)
        desired_location = [1,2,3] # top right of front 2x2 face
    piece = 1
    master_list = []
    for relevant_piece in relevant_pieces_copy:
        final_movelist = []
        iter = 0
        for move_list in valid_movelist:
            if iter % 100 == 0:
                clear_terminal()
                print(f'Evaluating path {iter} / {len(valid_movelist)} at max depth {init_maxdepth}. Piece # {piece}')
            iter += 1
            newcube = magiccube.Cube(cube_size, solved_state)
            newcube.get_piece((relevant_piece[0], relevant_piece[1], relevant_piece[2])).set_piece_color(get_axis(relevant_piece), Color.create("W"))
            newcube.get_piece((2,2,3)).set_piece_color(get_axis([2,2,3]), Color.create("W")) # set last solved piece white
            for move in move_list:
                newcube.rotate(move)
    
            if str(newcube.cube[desired_location[0], desired_location[1], desired_location[2]]) == "W" and str(newcube.cube[2,2,3]) == "W": # solved without messing up last solved piece
                final_movelist = move_list
                break
        piece += 1 
        list_el = [relevant_piece, final_movelist]  
        master_list.append(list_el)
    print(f'Master list: {master_list} ')

#top_right_white_face()
top_left_white_face()



    



