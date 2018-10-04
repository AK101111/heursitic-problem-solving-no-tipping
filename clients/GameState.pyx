# disutils: language = c++
from GameState cimport GameState

cdef class PyGameState:
    cdef GameState cgameState

    def __init__(self):
        self.cgameState = GameState()
    
    def print(self):
        self.print()
