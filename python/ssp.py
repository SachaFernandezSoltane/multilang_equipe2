import os
import sys
import random

class SSP:
    MAX_SOLUTIONS = 10000000

    def __init__(self, arg):
        self.solutions = []
        if isinstance(arg, int):
            n = arg
            if n <= 0:
                raise ValueError("SSP size cannot be nonpositive")
            if n <= 2:
                raise ValueError("SSP size is too small")
            self.target = 1
            self.original = [0] * n
            self.original[0] = 1
            for i in range(1, n):
                self.original[i] = i + 1
                if random.choice([True, False]):
                    self.target += self.original[i]
            # Tri décroissant pour améliorer l'élagage
            self.original.sort(reverse=True)
        elif isinstance(arg, str):
            filename = arg
            if filename is None:
                raise ValueError("Given path/file is null")
            if not os.path.exists(filename):
                raise ValueError("Given path/file does not exist")
            with open(filename, 'r') as f:
                lines = f.read().split()
            if len(lines) < 2:
                raise ValueError("Error while parsing input file")
            size = int(lines[0])
            if size <= 0:
                raise ValueError("Error while parsing input file")
            self.original = [0] * size
            self.target = int(lines[1])
            if len(lines) < 2 + size:
                raise ValueError("Error while parsing input file")
            for i in range(size):
                self.original[i] = int(lines[2 + i])
            # Tri décroissant pour améliorer l'élagage
            self.original.sort(reverse=True)
        else:
            raise TypeError("Invalid argument type for SSP constructor")

    def total_sum(self):
        return sum(self.original)

    def bp(self, i=0, partial=0, total=None, x=None):
        if total is None:
            total = self.total_sum()
        if x is None:
            x = [False] * len(self.original)
        if partial + total < self.target or partial > self.target:
            return
        if partial == self.target:
            if len(self.solutions) >= self.MAX_SOLUTIONS:
                return
            sol = [self.original[k] for k in range(i) if x[k]]
            self.solutions.append(sol)
            return
        if i == len(self.original):
            return
        total -= self.original[i]
        x[i] = False
        self.bp(i + 1, partial, total, x)
        x[i] = True
        partial += self.original[i]
        self.bp(i + 1, partial, total, x)
        x[i] = False

    def __str__(self):
        return f"SSP(n = {len(self.original)}; target = {self.target})"

    def show_target(self):
        return f"Target is {self.target}"

    def show_integers(self):
        s = "Original set = ["
        s += ",".join(str(x) for x in self.original)
        s += "]"
        return s

def print_usage():
    print("Usage: python SSP.py <integer|filename|directory>")

def is_integer(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

def process_ssp(ssp):
    print(ssp)
    print(ssp.show_integers())
    print(ssp.show_target())
    print()
    print("Running bp ... ", end="")
    import time
    start = time.time()
    ssp.bp()
    end = time.time()
    print("done!")
    if len(ssp.solutions) < 10:
        print(ssp.solutions)
    else:
        print(f"bp found {len(ssp.solutions)} solutions")
    print(f"elapsed time {int((end - start) * 1000)}")
    print()

def process_directory(dir_path):
    if not os.path.isdir(dir_path):
        print(f"Le répertoire est vide ou inaccessible: {dir_path}", file=sys.stderr)
        return
    for filename in os.listdir(dir_path):
        if filename.endswith(".txt"):
            print(f"Traitement du fichier : {filename}")
            try:
                ssp = SSP(os.path.join(dir_path, filename))
                process_ssp(ssp)
            except Exception as e:
                print(f"Erreur lors du traitement du fichier {filename} : {e}", file=sys.stderr)

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print_usage()
        sys.exit(0)
    arg = sys.argv[1]
    if is_integer(arg):
        ssp = SSP(int(arg))
        process_ssp(ssp)
    elif os.path.exists(arg):
        if os.path.isfile(arg):
            ssp = SSP(arg)
            process_ssp(ssp)
        elif os.path.isdir(arg):
            process_directory(arg)
        else:
            print(f"Argument is not a file or directory: {arg}", file=sys.stderr)
    else:
        print(f"Argument is not an integer or a valid file/directory: {arg}", file=sys.stderr)
