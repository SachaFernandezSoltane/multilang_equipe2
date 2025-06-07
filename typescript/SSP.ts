
import * as fs from "fs";
import * as path from "path";

class SSP {
  public target: number;
  public original: number[];
  public solutions: number[][] = [];

  constructor(target: number, original: number[]) {
    this.target = target;
    this.original = original;
  }

  public static createRandom(n: number): SSP {
    if (n <= 2) throw new Error("SSP size must be > 2");

    const original = Array.from({ length: n }, (_, i) => i + 1);
    let target = 1;
    for (let i = 1; i < n; i++) {
      if (Math.random() < 0.5) {
        target += original[i];
      }
    }

    // Tri décroissant
    original.sort((a, b) => b - a);

    return new SSP(target, original);
  }

  public static createFromFile(filename: string): SSP {
    const text = fs.readFileSync(filename, "utf8").trim();
    const lines = text.split(/\r?\n/);
    const size = parseInt(lines[0], 10);
    const target = parseInt(lines[1], 10);
    const parts = lines[2].trim().split(/\s+/).map(Number);
    const original = parts.slice(0, size);

    // Tri décroissant pour optimiser la prune
    original.sort((a, b) => b - a);

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

  public runBP(): void {
    const x = new Array(this.original.length).fill(false);
    this.bpRecursive(0, 0, this.totalSum(), x);
  }

  public printSolutions(): void {
    console.log(`SSP(n = ${this.original.length}; target = ${this.target})`);
    console.log(`Original set = [${this.original.join(", ")}]`);
    console.log(`Target is ${this.target}\n`);

    console.log("Running bp...");
    const start = Date.now();
    this.runBP();
    const end = Date.now();
    console.log("done!");

    if (this.solutions.length < 10) {
      for (const sol of this.solutions) {
        console.log(`[${sol.join(", ")}]`);
      }
    } else {
      console.log(`bp found ${this.solutions.length} solutions`);
    }
    console.log(`elapsed time: ${end - start} ms`);
  }
}

// --- MAIN CLI ---
function runSSP(input: string) {
  if (!isNaN(Number(input))) {
    // Cas 1 : nombre → instance aléatoire
    const n = parseInt(input, 10);
    const s = SSP.createRandom(n);
    s.printSolutions();
    return;
  }

  if (!fs.existsSync(input)) {
    console.error("Error: File or directory does not exist.");
    process.exit(1);
  }

  const stat = fs.statSync(input);

  if (stat.isFile()) {
    // Cas 2 : fichier .txt
    const s = SSP.createFromFile(input);
    s.printSolutions();
  } else if (stat.isDirectory()) {
    // Cas 3 : dossier → tous les .txt
    const files = fs.readdirSync(input).filter(f => f.endsWith(".txt"));
    for (const fname of files) {
      const full = path.join(input, fname);
      console.log(`=== Traitement de : ${full} ===`);
      const s = SSP.createFromFile(full);
      s.printSolutions();
      console.log();
    }
  } else {
    console.error("Unsupported path type.");
  }
}

// Lancement CLI
const args = process.argv.slice(2);
if (args.length < 1) {
  console.log("Usage: node SSP.js <size|filename|directory>");
  process.exit(1);
}
runSSP(args[0]);
