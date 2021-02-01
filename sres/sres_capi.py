import os
import glob
import ctypes as ct
import sys
from typing import List


class _SRESLoader:

    def __init__(self):
        self._lib = self._load_lib()

    def _get_shared_library_extension(self):
        if sys.platform == "linux":
            return ".so"
        elif sys.platform == "win32":
            return ".dll"
        elif sys.platform == "darwin":
            return ".dylib"

    def _get_shared_library_prefix(self):
        if sys.platform == "linux":
            return "lib"
        elif sys.platform == "win32":
            return ""
        elif sys.platform == "darwin":
            return "lib"

    def _load_lib(self):
        """Load the SRES C API binary"""
        lib = ct.CDLL(f"{self._get_shared_library_prefix()}SRES{self._get_shared_library_extension()}")
        return lib

    def _load_func(self, funcname: str, argtypes: List, return_type) -> ct.CDLL._FuncPtr:
        """Load a single function from SRES shared library"""
        func = self._lib.__getattr__(funcname)
        func.restype = return_type
        func.argtypes = argtypes
        return func



