# disutils: language = c++
from time import sleep
import game

x = input()
x = int(x)

gameState = game.PyGameState()
board_state = [0]*60
board_state[-26] = 3
position, weight = gameState.play(x)
