#!/usr/bin/python
import os
import unittest
from subprocess import check_output as qx

"""
#dictionary
name
home
temporary

#test cases
#exactly match
n a m e => name

#returns a candidate that contains all letters & has the smallest distance
o m e => home

#returns a candidate that contains all letters & has the smallest distance
r o m e => temporary

#two candidates that has the same distance
m e => name home

#No candidates that contains all letters
a b => null
"""

"""
#dictionary
borrow

#cases
bow => borrow
borrow => borrow
borroww => null

"""

fbin = "./allINone"
fdict = "dict.txt"


class TestMethods(unittest.TestCase):

    def letters_in_word(self, letter_word, words, distance=10):
        l = " ".join(c for c in letter_word)
        cmd = [fbin, "-d", fdict, "-m", "l2w", "-l", str(distance), "-w", l]
        output = qx(cmd)
        o = output.strip().split("\n")
        self.assertEqual(o, words)

    def letters_notin_word(self, letter_word, distance=10):
        l = " ".join(c for c in letter_word)
        cmd = [fbin, "-d", fdict, "-m", "l2w", "-l", str(distance), "-w", l]
        output = qx(cmd)
        o = output.strip().split("\n")
        self.assertEqual(o, [""])

    def test_l2w_exactly(self):
        w = "name"
        self.letters_in_word(w, ["name"])

        w = "borrow"
        self.letters_in_word(w, ["borrow"])

    def test_l2w_contain(self):
        w = "ome"
        self.letters_in_word(w, ["home"])

        w = "bow"
        self.letters_in_word(w, ["borrow"], 3)
        self.letters_notin_word(w, 2)

    def test_l2w_contain2(self):
        w = "me"
        self.letters_in_word(w, ["name", "home"], 2)
        self.letters_notin_word(w, 1)

    def test_l2w_notcontain(self):
        w = "ab"
        self.letters_notin_word(w)

        w = "borroww"
        self.letters_notin_word(w)

    def word_in_letters(self, letter_word, words, distance=10):
        l = " ".join(c for c in letter_word)
        cmd = [fbin, "-d", fdict, "-m", "w2l", "-l", str(distance), "-w", l]
        output = qx(cmd)
        o = output.strip().split("\n")
        self.assertEqual(o, words)

    def word_notin_letters(self, letter_word, distance=10):
        l = " ".join(c for c in letter_word)
        cmd = [fbin, "-d", fdict, "-m", "w2l", "-l", str(distance), "-w", l]
        output = qx(cmd)
        o = output.strip().split("\n")
        self.assertEqual(o, [""])

    def test_w2l_exactly(self):
        w = "name"
        self.word_in_letters(w, ["name"])

        w = "borrow"
        self.word_in_letters(w, ["borrow"])

    def test_w2l_contain(self):
        w = "homeww"
        self.word_in_letters(w, ["home"], 2)
        self.word_notin_letters(w, 1)

    def test_w2l_contain2(self):
        w = "nameho"
        self.word_in_letters(w, ["name", "home"], 2)
        self.word_notin_letters(w, 1)

    def test_zzzz(self):
        if os.path.exists(fdict):
            os.remove(fdict)


def gen_dictionary(dict_path):
    dict = \
    "name\n" \
    "home\n" \
    "temporary\n" \
    "borrow\n"

    with open(dict_path, "w") as f:
        f.write(dict)


if __name__ == "__main__":
    if not os.path.exists(fbin):
        print "Binary file %s not exist!" % (fbin)

    # if not os.path.exists(fdict):
    #    gen_dictionary(fdict)
    gen_dictionary(fdict)

    unittest.main()
