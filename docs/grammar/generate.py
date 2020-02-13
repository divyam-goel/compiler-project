import re
import json
from typing import *
from collections import OrderedDict

GRAMMAR_FILE = "text/grammar.txt"
EPSILON = "\u03b5"


def pretty_print(target):
    print(json.dumps(target, indent=4))


def obtain_grammar(filename):
    raw_grammar = None
    refined_grammar = {}
    with open(filename, "r") as f:
        raw_grammar = f.readlines()
    for rule in raw_grammar:
        if rule.strip():
            lhs, rhs = [side.strip() for side in rule.split("->")]
            refined_grammar[lhs] = []
            for sub_rule in rhs.split("|"):
                term_stream = [x.strip() for x in sub_rule.split()]
                refined_grammar[lhs].append(term_stream)
    return refined_grammar


def get_first_sets(grammar):
    first_sets = OrderedDict()

    def get_first_set(nt):
        first_set = []

        for rule in grammar[nt]:
            for index, term in enumerate(rule):

                # Base case - terminal
                if not term.startswith("<"):
                    first_set.append(term)
                    break

                # Avoid infinite recursion in the presence of left-recursion.
                if term == nt:
                    break

                _first_set = get_first_set(term)
                # There seems to be potential for DP here, but when I tried implementing
                # it I ended up with a few issues (namely missing epsilons). So if anyone
                # else wants to add it, feel free to (I don't have the time right now).
                if EPSILON in _first_set:
                    if index < len(rule) - 1:
                        _first_set.remove(EPSILON)
                    first_set += _first_set
                    # Now if this term was not the last then go for
                    # another iteration and look at the next term.
                else:
                    # This non-terminal definitely derives its own first symbol.
                    first_set = list(set(first_set).union(set(_first_set)))
                    break

        return first_set

    for nt in grammar:
        first_sets[nt] = get_first_set(nt)

    return first_sets


if __name__ == "__main__":
    grammar = obtain_grammar(GRAMMAR_FILE)
    first_sets = get_first_sets(grammar)
    pretty_print(first_sets)

