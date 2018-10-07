from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(ext_modules = cythonize(Extension("game", sources=["GameState.pyx", "GameState.C"], language="c++")))
