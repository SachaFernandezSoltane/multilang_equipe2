
import * as fs from 'fs';

class SSP {
  private target: number;
  private original: number[];
  private solutions: number[][] = [];

  constructor(target: number, original: number[]) {
    this.target = target;
    this.original = original;
  }

  static createRandom(n: number): SSP {
    if (n <= 0) throw new Error("SSP size cannot be nonpositive");
    if (n <= 2) throw new Error("SSP size is too small");

    const original = Array.from({ length: n }, (_, i) => i + 1);
    let target = 1;
    for (let i = 1; i < n; i++) {
      if (Math.random() < 0.5) {
        target += original[i];
      }
    }

    return new SSP(target, original);
  }

static createFromFile(filename: string): SSP {
  // Lire tout le contenu, séparer en lignes
  const lines = fs.readFileSync(filename, "utf8")
    .trim()
    .split(/\r?\n/);

  // Ligne 1 : taille
  const size = parseInt(lines[0], 10);
  // Ligne 2 : target
  const target = parseInt(lines[1], 10);
  // Ligne 3 (unique) : liste d'entiers séparés par des espaces
  const original = lines[2]
    .trim()
    .split(/\s+/)
    .slice(0, size)    // on prend exactement `size` valeurs
    .map((s) => parseInt(s, 10));

  return new SSP(target, original);
}


  private totalSum(): number {
    return this.original.reduce((a, b) => a + b, 0);
  }

  private bpRecursive(i: number, partial: number, total: number, x: boolean[]) {
    if (partial + total < this.target || partial > this.target) return;

    if (partial === this.target) {
      const solution = this.original.filter((_, idx) => x[idx]);
      this.solutions.push(solution);
      return;
    }

    if (i >= this.original.length) return;

    const newTotal = total - this.original[i];

    x[i] = false;
    this.bpRecursive(i + 1, partial, newTotal, x);

    x[i] = true;
    this.bpRecursive(i + 1, partial + this.original[i], newTotal, x);

    x[i] = false;
  }

  runBP() {
    const x = Array(this.original.length).fill(false);
    this.bpRecursive(0, 0, this.totalSum(), x);
  }

  showTarget(): string {
    return `Target is ${this.target}`;
  }

  showIntegers(): string {
    return `Original set = [${this.original.join(', ')}]`;
  }

  toString(): string {
    return `SSP(n = ${this.original.length}; target = ${this.target})`;
  }

  printSolutions(): void {
    if (this.solutions.length < 10) {
      for (const sol of this.solutions) {
        console.log(`[${sol.join(', ')}]`);
      }
    } else {
      console.log(`bp found ${this.solutions.length} solutions`);
    }
  }
}

// --- main ---
function runSSP(input: string) {
  let ssp: SSP;
  if (/^\d+$/.test(input)) {
    const n = parseInt(input);
    ssp = SSP.createRandom(n);
  } else if (fs.existsSync(input)) {
    ssp = SSP.createFromFile(input);
  } else {
    console.error("Argument must be an integer or a valid filename");
    return;
  }

  console.log(ssp.toString());
  console.log(ssp.showIntegers());
  console.log(ssp.showTarget());
  console.log();

  console.log("Running bp...");
  const start = Date.now();
  ssp.runBP();
  const end = Date.now();
  console.log("done!");

  ssp.printSolutions();
  console.log(`elapsed time: ${end - start} ms`);
}

const args = process.argv.slice(2);
runSSP(args[0]);
