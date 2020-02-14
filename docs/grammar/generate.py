""" Just a quick disclaimer: I wasn't really focusing on achieving the best
possible efficiency here. My only focus was on correctness and I achieved that.
I didn't have the time to spend on efficiency where it wasn't required. deal
with it (I'm mostly just telling this to myself here). """

import re
import json
from typing import *
from collections import OrderedDict

GRAMMAR_FILE = "text/grammar.txt"
EPSILON = "\u03b5"

grammar_t = Dict[str, List[List[str]]]
ff_set_t = Dict[str, List[str]]

def pretty_print(target: Dict[str, Any], filename: Optional[str]=None) -> None:
    data = json.dumps(target, indent=4)
    if filename:
        with open(filename, "w+") as outp:
            outp.write(data)
    else:
        print(data)


def get_dependant_rules(nt: str, grammar: grammar_t) -> List[Tuple[str, int]]:
    dependant_rules = []
    for lhs, rule in grammar.items():
        for idx, sub_rule in enumerate(rule):
            if nt in sub_rule:
                dependant_rules.append((lhs, idx))
    return dependant_rules


def obtain_grammar(filename: str) -> grammar_t:
    raw_grammar = None
    refined_grammar = {}  # type: grammar_t
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


def get_first_sets(grammar: grammar_t) -> ff_set_t:
    first_sets = OrderedDict()  # type: ff_set_t

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

                ext_first_set = get_first_set(term)
                if EPSILON in ext_first_set:
                    if index < len(rule) - 1:
                        ext_first_set.remove(EPSILON)
                    first_set += ext_first_set
                    # Now if this term was not the last then go for
                    # another iteration and look at the next term.
                else:
                    # This non-terminal definitely derives its own first symbol.
                    first_set = list(set(first_set).union(set(ext_first_set)))
                    break

        return first_set

    for nt in grammar:
        first_sets[nt] = get_first_set(nt)

    return first_sets


def get_follow_sets(grammar: grammar_t, first_sets: ff_set_t) -> ff_set_t:
    follow_sets = OrderedDict()  # type: ff_set_t

    start_symbol = list(grammar.keys())[0]  # grammar is an OrderedDict
    follow_sets[start_symbol] = ["$"]

    def get_follow_set(nt):
        follow_set = []
        if nt == start_symbol:
            follow_set += "$"
        dependant_rules = get_dependant_rules(nt, grammar)

        for lhs, sub_rule_idx in dependant_rules:
            cur = 0
            eps_flag = False
            while(cur < len(grammar[lhs][sub_rule_idx])):
                if not eps_flag:
                    if grammar[lhs][sub_rule_idx][cur] != nt:
                        cur += 1
                        continue
                else:
                    eps_flag = False

                cur += 1

                if cur == len(grammar[lhs][sub_rule_idx]):
                    if lhs == nt:
                        # convergence condition
                        continue
                    follow_set += get_follow_set(lhs)

                else:
                    term = grammar[lhs][sub_rule_idx][cur]

                    # Base case - terminal.
                    if not term.startswith("<"):
                        follow_set.append(term)
                        continue

                    first_set = first_sets[term]
                    follow_set += first_set
                    if EPSILON in follow_set:
                        follow_set.remove(EPSILON)
                        eps_flag = True

        return list(set(follow_set))

    for nt in grammar.keys():
        follow_sets[nt] = get_follow_set(nt)

    return follow_sets

if __name__ == "__main__":
    grammar = obtain_grammar(GRAMMAR_FILE)
    first_sets = get_first_sets(grammar)
    pretty_print(first_sets, "first_sets.txt")
    follow_sets = get_follow_sets(grammar, first_sets)
    pretty_print(follow_sets, "follow_sets.txt")

