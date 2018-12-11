# Configuration

UNALLOWED_HEADERS = [
    "array",
    "bitset",
    "deque",
    "forward_list",
    "list",
    "map",
    "queue",
    "set",
    "stack",
    "unordered_map",
    "unordered_set",
    "vector",
    "atomic",
    "condition_variable",
    "future",
    "mutex",
    "thread",
    "algorithm",
    "chrono",
    "codecvt",
    "complex",
    "exception",
    "functional",
    "initializer_list",
    "iterator",
    "limits",
    "locale",
    "memory",
    "new",
    "numeric",
    "random",
    "ratio",
    "regex",
    "stdexcept",
    "string",
    "system_error",
    "tuple",
    "typeindex",
    "typeinfo",
    "type_traits",
    "utility",
    "valarray"
]
UNALLOWED_HEADER_TEMPLATE = """
#ifndef {0}_WAS_INCLUDED
#define {0}_WAS_INCLUDED
#error You are not allowed to use STL headers ({0})
#endif
"""

FORWARD_HEADERS = [
    "cassert",
    "cctype",
    "cerrno",
    "cfenv",
    "cfloat",
    "cinttypes",
    "ciso646",
    "climits",
    "clocale",
    "cmath",
    "csetjmp",
    "csignal",
    "cstdarg",
    "cstdbool",
    "cstddef",
    "cstdint",
    "cstdio",
    "cstdlib",
    "cstring",
    "ctgmath",
    "ctime",
    "cuchar",
    "cwchar",
    "cwctype"
]
FORWARD_HEADER_TEMPLATE = """
#include <{0}>
"""

# Execution
for unallowed_header in UNALLOWED_HEADERS:
    with open(unallowed_header, "w") as text_file:
        text_file.write(UNALLOWED_HEADER_TEMPLATE.format(unallowed_header))

#for forward_header in FORWARD_HEADERS:
#    with open(forward_header, "w") as text_file:
#        text_file.write(FORWARD_HEADER_TEMPLATE.format(forward_header[1:] + ".h"))
