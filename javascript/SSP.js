import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);

class SSP {
  constructor(input) {
    this.solutions = [];

    if (typeof input === 'number') {
      if (input <= 2) throw new Error('SSP size is too small');

      this.original = new Array(input);
      this.original[0] = 1;
      this.target = 1;

      for (let i = 1; i < input; i++) {
        this.original[i] = i + 1;
        if (Math.random() < 0.5) this.target += this.original[i];
      }

      this.original.sort((a, b) => b - a); // Descending
    } else if (typeof input === 'string') {
      const content = fs.readFileSync(input, 'utf-8');
      const lines = content.trim().split(/\r?\n/);

      if (lines.length < 3) throw new Error('Invalid input file');
      const size = parseInt(lines[0]);
      if (isNaN(size) || size <= 0) throw new Error('Invalid size');

      this.target = parseInt(lines[1]);
      if (isNaN(this.target)) throw new Error('Invalid target');

      // Les entiers sont supposés sur une ligne séparée, éventuellement plusieurs lignes concaténées
      const nums = lines.slice(2).join(' ').trim().split(/\s+/).map(Number);
      if (nums.length < size) throw new Error('Not enough integers');

      this.original = nums.slice(0, size);
      this.original.sort((a, b) => b - a);
    } else {
      throw new Error('Invalid constructor argument');
    }
  }

  totalSum() {
    return this.original.reduce((acc, val) => acc + val, 0);
  }

  bpRecursive(i, partial, total, x) {
    if (partial + total < this.target || partial > this.target) return;

    if (partial === this.target) {
      if (this.solutions.length >= 10000000) return;
      const sol = [];
      for (let k = 0; k < i; k++) if (x[k]) sol.push(this.original[k]);
      this.solutions.push(sol);
      return;
    }

    if (i === this.original.length) return;

    total -= this.original[i];

    x[i] = false;
    this.bpRecursive(i + 1, partial, total, x);

    x[i] = true;
    this.bpRecursive(i + 1, partial + this.original[i], total, x);
    x[i] = false;
  }

  bp() {
    const x = new Array(this.original.length).fill(false);
    this.bpRecursive(0, 0, this.totalSum(), x);
  }

  toString() {
    return `SSP(n = ${this.original.length}; target = ${this.target})`;
  }

  showTarget() {
    return `Target is ${this.target}`;
  }

  showIntegers() {
    return `Original set = [${this.original.join(', ')}]`;
  }
}

// CLI : détecte si ce module est le point d'entrée
if (process.argv[1] && path.resolve(process.argv[1]) === path.resolve(__filename)) {
  const arg = process.argv[2];
  if (!arg) {
    console.log('Usage: node ssp.js <integer|filename|directory>');
    process.exit(1);
  }

  if (!isNaN(arg)) {
    const ssp = new SSP(parseInt(arg));
    processSSP(ssp);
  } else if (fs.existsSync(arg)) {
    const stat = fs.statSync(arg);
    if (stat.isFile()) {
      const ssp = new SSP(arg);
      processSSP(ssp);
    } else if (stat.isDirectory()) {
      const files = fs.readdirSync(arg);
      for (const file of files) {
        if (file.endsWith('.txt')) {
          console.log(`Traitement du fichier : ${file}`);
          try {
            const ssp = new SSP(path.join(arg, file));
            processSSP(ssp);
          } catch (e) {
            console.error(`Erreur pour ${file} : ${e.message}`);
          }
        }
      }
    }
  } else {
    console.error('Invalid argument');
  }
}

function processSSP(ssp) {
  console.log(ssp.toString());
  console.log(ssp.showIntegers());
  console.log(ssp.showTarget());
  console.log();

  console.log('Running bp ...');
  const start = Date.now();
  ssp.bp();
  const end = Date.now();
  console.log('done!');
  if (ssp.solutions.length < 10) {
    console.log(ssp.solutions);
  } else {
    console.log(`bp found ${ssp.solutions.length} solutions`);
  }
  console.log(`elapsed time ${end - start} ms\n`);
}
