import cython
from libcpp.vector cimport vector
from libcpp.utility cimport pair

cdef extern from "GameState.C":
    pass

# Decalre the class with cdef
cdef extern from "GameState.H" namespace "rtt":
    cdef cppclass GameState:
        GameState()
        void absorb(const vector[int]& boardState)
        pair[int, int] play()
        void print() const

cdef class PyGameState:
    cdef GameState cgameState

    def __init__(self):
        self.cgameState = GameState()

    def print(self):
        self.print()
