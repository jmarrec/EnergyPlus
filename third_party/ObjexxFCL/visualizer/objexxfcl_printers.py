# -*- coding: utf-8 -*-
# GDB pretty printers for ObjexxFCL's owning array types (Array1D, Array2D,
# Array3D, Array4D, ...), so `print`/`p` shows a readable "(lower:upper, ...)"
# shape and the element values instead of the raw internal layout
# (owner_/capacity_/mem_/data_/shift_/sdata_/I_/z1_/...).
#
# To use, from a running gdb session:
#
#   (gdb) source /path/to/EnergyPlus/third_party/ObjexxFCL/visualizer/objexxfcl_printers.py
#
# Or add that same line to ~/.gdbinit to have it load automatically every
# session:
#
#   source /path/to/EnergyPlus/third_party/ObjexxFCL/visualizer/objexxfcl_printers.py
#
# Either way, `print someArray1D` now prints something like:
#
#        $1 = ObjexxFCL::Array1D<std::string>(1:1463) = {"a", "b", "c"...}
#
#      and for multi-dimensional arrays, each element is labeled with its
#      Fortran-style (i1, i2, ...) subscript, e.g. Array2D(1:2, 1:3):
#
#        $1 = ObjexxFCL::Array2D<double>(1:2, 1:3) = {
#          (1, 1) = 0,
#          (1, 2) = 0,
#          (1, 3) = 0,
#          (2, 1) = 0,
#          (2, 2) = 0,
#          (2, 3) = 0
#        }
#
# If your gdb's "set print pretty"/"set print array" defaults are off, the
# same array may instead print all on one line (or otherwise look flat/
# unindented). Either flip those globally:
#
#   (gdb) set print pretty on
#   (gdb) set print array on
#
# or force it for a single call with the print command's per-invocation
# options:
#
#   (gdb) print -pretty on -array on -- someArray1D
#
# Handles ArrayND, ArrayND (owning) and ArrayNA (argument/borrowed) for
# N = 1..6 -- that's every type that shares ArrayFCL's ArrayN<T> base layout
# (I_/I1_../z1_../size_/data_/sdata_), which covers "Array1D<T> x;" style
# declarations used everywhere in this codebase. Also handles ArrayNS
# slice/proxy types (Array1S, Array2S, ...), which derive from ArrayRS
# instead and use a different (data_/k_/m1_../u1_../size_) layout -- e.g.
# the Alphas/Numbers arguments in InputProcessor::getObjectItem.

import gdb
import re
import itertools

_ARRAY_TYPE_RE = re.compile(r"ObjexxFCL::Array(\d+)[DA]?<")


class ObjexxArrayPrinter:
    "Print an ObjexxFCL::ArrayND (Array1D, Array2D, Array3D, Array4D, ...)"

    def __init__(self, val):
        self.val = val
        t = val.type
        if t.code == gdb.TYPE_CODE_REF:
            t = t.target()
        t = t.strip_typedefs()
        tag = t.tag if t.tag else str(t)

        m = _ARRAY_TYPE_RE.match(tag)
        self.dims = int(m.group(1)) if m else 1

        self.elem_type = t.template_argument(0)

        self.bounds = []  # [(l1, u1), (l2, u2), ...]
        self.z = []  # [z2, z3, ..., zN] strides used by operator()
        if self.dims == 1:
            I = val["I_"]
            self.bounds.append((int(I["l_"]), int(I["u_"])))
        else:
            for d in range(1, self.dims + 1):
                I = val["I%d_" % d]
                self.bounds.append((int(I["l_"]), int(I["u_"])))
            for d in range(2, self.dims + 1):
                self.z.append(int(val["z%d_" % d]))

        self.size = int(val["size_"])

        try:
            data = val["sdata_"]
        except gdb.error:
            data = val["data_"]
        self.data = data.cast(self.elem_type.pointer())

    def _offset(self, idx):
        # Mirrors ObjexxFCL::ArrayN::operator()(i1, i2, ...):
        # sdata_[((i1 * z2) + i2) * z3 + i3 ...]
        off = idx[0]
        for i, z in zip(idx[1:], self.z):
            off = (off * z) + i
        return off

    def to_string(self):
        shape = ", ".join("%d:%d" % (l, u) for (l, u) in self.bounds)
        return "%s(%s)" % (self.val.type, shape)

    def children(self):
        if self.size == 0:
            return
        ranges = [range(l, u + 1) for (l, u) in self.bounds]
        for idx in itertools.product(*ranges):
            off = self._offset(idx)
            name = idx[0] if self.dims == 1 else "(%s)" % ", ".join(str(i) for i in idx)
            yield (str(name), (self.data + off).dereference())


_SLICE_TYPE_RE = re.compile(r"ObjexxFCL::Array(\d+)S<")


class ObjexxSlicePrinter:
    "Print an ObjexxFCL::ArrayNS slice/proxy (Array1S, Array2S, ...)"

    def __init__(self, val):
        self.val = val
        t = val.type
        if t.code == gdb.TYPE_CODE_REF:
            t = t.target()
        t = t.strip_typedefs()
        tag = t.tag if t.tag else str(t)

        m = _SLICE_TYPE_RE.match(tag)
        self.dims = int(m.group(1))

        self.elem_type = t.template_argument(0)

        self.k = int(val["k_"])
        self.u = []  # [u1, u2, ..., uN]
        self.m = []  # [m1, m2, ..., mN]
        if self.dims == 1:
            self.u.append(int(val["u_"]))
            self.m.append(int(val["m_"]))
        else:
            for d in range(1, self.dims + 1):
                self.u.append(int(val["u%d_" % d]))
                self.m.append(int(val["m%d_" % d]))

        self.size = int(val["size_"])
        self.data = val["data_"].cast(self.elem_type.pointer())

    def _offset(self, idx):
        # Mirrors ObjexxFCL::ArrayNS::operator()(i1, i2, ...):
        # data_[k_ + (m1 * i1) + (m2 * i2) + ...]
        off = self.k
        for i, m in zip(idx, self.m):
            off += m * i
        return off

    def to_string(self):
        shape = ", ".join("1:%d" % u for u in self.u)
        return "%s(%s)" % (self.val.type, shape)

    def children(self):
        if self.size == 0:
            return
        ranges = [range(1, u + 1) for u in self.u]
        for idx in itertools.product(*ranges):
            off = self._offset(idx)
            name = idx[0] if self.dims == 1 else "(%s)" % ", ".join(str(i) for i in idx)
            yield (str(name), (self.data + off).dereference())


def _objexxfcl_lookup(val):
    t = val.type
    if t.code == gdb.TYPE_CODE_REF:
        t = t.target()
    t = t.strip_typedefs()
    tag = t.tag if t.tag else ""
    if _ARRAY_TYPE_RE.match(tag):
        return ObjexxArrayPrinter(val)
    if _SLICE_TYPE_RE.match(tag):
        return ObjexxSlicePrinter(val)
    return None


gdb.pretty_printers.append(_objexxfcl_lookup)


class Ptrunc(gdb.Command):
    """One-off truncated print, handy for large ObjexxFCL arrays.

    ptrunc EXPR N          -- fixed limit
    ptrunc EXPR SOME_VAR    -- limit is SOME_VAR's current value (e.g. NumAlphas)
    """

    def __init__(self):
        super().__init__("ptrunc", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        parts = arg.split()
        if len(parts) < 2:
            print("usage: ptrunc EXPR N | ptrunc EXPR EXPR")
            return
        expr, limit_expr = parts[0], " ".join(parts[1:])
        try:
            limit = int(limit_expr)
        except ValueError:
            # Not a plain integer -- treat it as an expression (e.g. a
            # variable like NumAlphas) and evaluate it.
            try:
                limit = int(gdb.parse_and_eval(limit_expr))
            except gdb.error as e:
                print("error evaluating %r: %s" % (limit_expr, e))
                return
        try:
            val = gdb.parse_and_eval(expr)
            # max_characters=-1 (unlimited) keeps string *contents* full-length --
            # only the number of array/container *elements* shown is capped.
            print("%s = %s" % (expr, val.format_string(max_elements=limit, max_characters=-1)))
        except gdb.error as e:
            print("error: %s" % e)


Ptrunc()
