"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var fs = require("fs");
var path = require("path");
var SSP = /** @class */ (function () {
    function SSP(target, original) {
        this.solutions = [];
        this.target = target;
        this.original = original;
    }
    SSP.createRandom = function (n) {
        if (n <= 2)
            throw new Error("SSP size must be > 2");
        var original = Array.from({ length: n }, function (_, i) { return i + 1; });
        var target = 1;
        for (var i = 1; i < n; i++) {
            if (Math.random() < 0.5) {
                target += original[i];
            }
        }
        // Tri décroissant
        original.sort(function (a, b) { return b - a; });
        return new SSP(target, original);
    };
    SSP.createFromFile = function (filename) {
        var text = fs.readFileSync(filename, "utf8").trim();
        var lines = text.split(/\r?\n/);
        var size = parseInt(lines[0], 10);
        var target = parseInt(lines[1], 10);
        var parts = lines[2].trim().split(/\s+/).map(Number);
        var original = parts.slice(0, size);
        // Tri décroissant pour optimiser la prune
        original.sort(function (a, b) { return b - a; });
        return new SSP(target, original);
    };
    SSP.prototype.totalSum = function () {
        return this.original.reduce(function (a, b) { return a + b; }, 0);
    };
    SSP.prototype.bpRecursive = function (i, partial, total, x) {
        if (partial + total < this.target || partial > this.target)
            return;
        if (partial === this.target) {
            var solution = this.original.filter(function (_, idx) { return x[idx]; });
            this.solutions.push(solution);
            return;
        }
        if (i >= this.original.length)
            return;
        var newTotal = total - this.original[i];
        x[i] = false;
        this.bpRecursive(i + 1, partial, newTotal, x);
        x[i] = true;
        this.bpRecursive(i + 1, partial + this.original[i], newTotal, x);
        x[i] = false;
    };
    SSP.prototype.runBP = function () {
        var x = new Array(this.original.length).fill(false);
        this.bpRecursive(0, 0, this.totalSum(), x);
    };
    SSP.prototype.printSolutions = function () {
        console.log("SSP(n = ".concat(this.original.length, "; target = ").concat(this.target, ")"));
        console.log("Original set = [".concat(this.original.join(", "), "]"));
        console.log("Target is ".concat(this.target, "\n"));
        console.log("Running bp...");
        var start = Date.now();
        this.runBP();
        var end = Date.now();
        console.log("done!");
        if (this.solutions.length < 10) {
            for (var _i = 0, _a = this.solutions; _i < _a.length; _i++) {
                var sol = _a[_i];
                console.log("[".concat(sol.join(", "), "]"));
            }
        }
        else {
            console.log("bp found ".concat(this.solutions.length, " solutions"));
        }
        console.log("elapsed time: ".concat(end - start, " ms"));
    };
    return SSP;
}());
// --- MAIN CLI ---
function runSSP(input) {
    if (!isNaN(Number(input))) {
        // Cas 1 : nombre → instance aléatoire
        var n = parseInt(input, 10);
        var s = SSP.createRandom(n);
        s.printSolutions();
        return;
    }
    if (!fs.existsSync(input)) {
        console.error("Error: File or directory does not exist.");
        process.exit(1);
    }
    var stat = fs.statSync(input);
    if (stat.isFile()) {
        // Cas 2 : fichier .txt
        var s = SSP.createFromFile(input);
        s.printSolutions();
    }
    else if (stat.isDirectory()) {
        // Cas 3 : dossier → tous les .txt
        var files = fs.readdirSync(input).filter(function (f) { return f.endsWith(".txt"); });
        for (var _i = 0, files_1 = files; _i < files_1.length; _i++) {
            var fname = files_1[_i];
            var full = path.join(input, fname);
            console.log("=== Traitement de : ".concat(full, " ==="));
            var s = SSP.createFromFile(full);
            s.printSolutions();
            console.log();
        }
    }
    else {
        console.error("Unsupported path type.");
    }
}
// Lancement CLI
var args = process.argv.slice(2);
if (args.length < 1) {
    console.log("Usage: node SSP.js <size|filename|directory>");
    process.exit(1);
}
runSSP(args[0]);
