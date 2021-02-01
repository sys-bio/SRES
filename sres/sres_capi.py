import ctypes as ct
import glob
import os
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

    def _find_sres(self):
        """Find the SRES shared library"""
        this_directory = os.path.join(os.path.dirname(__file__))
        one_directory_up = os.path.dirname(this_directory)

        dlls = glob.glob(os.path.join(
            one_directory_up,
            f"**/{self._get_shared_library_prefix()}SRES{self._get_shared_library_extension()}")
        )

        if len(dlls) == 0:
            raise ValueError("SRES library not found")
        elif len(dlls) > 1:
            raise ValueError(f"Too may SRES libraries found: {dlls}")
        return dlls[0]

    def _load_lib(self):
        """Load the SRES C API binary"""
        shared_lib = self._find_sres()
        lib = ct.CDLL(shared_lib)
        return lib

    def _load_func(self, funcname: str, argtypes: List, return_type) -> ct.CDLL._FuncPtr:
        """Load a single function from SRES shared library"""
        func = self._lib.__getattr__(funcname)
        func.restype = return_type
        func.argtypes = argtypes
        return func
