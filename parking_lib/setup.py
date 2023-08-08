import pybind11
from distutils.core import setup, Extension

dir = str(pathlib.Path(__file__).parent.resolve())

ext_modules = [
Extension(
    'parking_lib', 
    [f'parking_lib/parking.cpp', f'parking_lib/main.cpp'], 
    include_dirs=[pybind11.get_include()], 
    language='c++',
    extra_compile_args=['-std=c++17'],
    ),
]

setup(
    name='parking_lib',
    ext_modules=ext_modules,
    requires=['pybind11']
    )