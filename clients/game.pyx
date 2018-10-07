import cython
from libcpp.vector cimport vector
from libcpp.utility cimport pair
import random

cdef extern from "GameState.H" namespace "rtt":
    cdef cppclass GameState:
        GameState() except+
        void absorb(const vector[int]& boardState)
        pair[int, int] play()
        void printBoard()

cdef class PyGameState:
    cdef GameState cgameState

    def __init__(self):
        self.cgameState = GameState()

    def printBoard(self):
        self.cgameState.printBoard()
    
    def absorb(self, board_state):
        cdef vector[int] vect = board_state
        self.cgameState.absorb(vect)

    def play(self):
        (index, wt) = self.cgameState.play()
        # wt negative means we are removing
        if wt == -1:
            # no sol found, play random
            if index == -100:
                return random.randint(-30,30)
            else:
                return index
        # addition
        else:
            if index == -100:
                print("playing random")
                return (random.randint(1, 10), random.randint(1,25))
            else:
                return (index, wt)
