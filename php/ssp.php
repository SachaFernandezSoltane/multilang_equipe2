<?php
// Usage CLI :
//   php ssp.php 15
//   php ssp.php inst_n20.txt
//   php ssp.php mon_dossier_txt

class SSP
{
    private int $target;
    private array $original;
    private array $solutions = [];

    public function __construct(int $target, array $original)
    {
        $this->target = $target;
        $this->original = $original;
    }

    public static function createRandom(int $n): SSP
    {
        if ($n <= 2) {
            throw new InvalidArgumentException("SSP size must be greater than 2");
        }
        $original = range(1, $n);
        $target = 1;
        for ($i = 1; $i < $n; $i++) {
            if (random_int(0, 1)) {
                $target += $original[$i];
            }
        }
        return new SSP($target, $original);
    }

    public static function createFromFile(string $filename): SSP
    {
        if (!file_exists($filename)) {
            throw new InvalidArgumentException("File not found");
        }
        $lines = file($filename, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        $size   = (int)$lines[0];
        $target = (int)$lines[1];
        $parts  = preg_split('/\s+/', trim($lines[2]));
        $original = array_map('intval', array_slice($parts, 0, $size));

        // Tri décroissant pour optimiser la prune
        rsort($original, SORT_NUMERIC);

        return new SSP($target, $original);
    }

    private function totalSum(): int
    {
        return array_sum($this->original);
    }

    private function bpRecursive(int $i, int $partial, int $total, array &$x): void
    {
        if ($partial + $total < $this->target || $partial > $this->target) {
            return;
        }
        if ($partial === $this->target) {
            $solution = [];
            foreach ($x as $idx => $used) {
                if ($used) {
                    $solution[] = $this->original[$idx];
                }
            }
            $this->solutions[] = $solution;
            return;
        }
        if ($i >= count($this->original)) {
            return;
        }
        $total -= $this->original[$i];

        $x[$i] = false;
        $this->bpRecursive($i + 1, $partial, $total, $x);

        $x[$i] = true;
        $this->bpRecursive($i + 1, $partial + $this->original[$i], $total, $x);

        $x[$i] = false;
    }

    public function runBP(): void
    {
        $x = array_fill(0, count($this->original), false);
        $this->bpRecursive(0, 0, $this->totalSum(), $x);
    }

    public function print(): void
    {
        echo "SSP(n = " . count($this->original) . "; target = {$this->target})\n";
        echo "Original set = [" . implode(', ', $this->original) . "]\n";
        echo "Target is {$this->target}\n\n";

        echo "Running bp...\n";
        $start = microtime(true);
        $this->runBP();
        $end = microtime(true);
        echo "done!\n";

        if (count($this->solutions) < 10) {
            foreach ($this->solutions as $sol) {
                echo "[" . implode(", ", $sol) . "]\n";
            }
        } else {
            echo "bp found " . count($this->solutions) . " solutions\n";
        }

        $elapsed = round(($end - $start) * 1000);
        echo "elapsed time: {$elapsed} ms\n";
    }
}

// --- MAIN CLI ---
$arg = $argv[1] ?? null;
if ($arg === null) {
    echo "Usage: php ssp.php <size|filename|directory>\n";
    exit(1);
}

try {
    if (is_numeric($arg)) {
        // Cas entier → instance aléatoire
        $ssp = SSP::createRandom((int)$arg);
        $ssp->print();
    }
    else if (is_file($arg)) {
        // Cas fichier unique
        $ssp = SSP::createFromFile($arg);
        $ssp->print();
    }
    else if (is_dir($arg)) {
        // Cas dossier : traiter tous les .txt
        $files = glob(rtrim($arg, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR . "*.txt");
        foreach ($files as $file) {
            echo "=== Traitement de : $file ===\n";
            $ssp = SSP::createFromFile($file);
            $ssp->print();
            echo "\n";
        }
    }
    else {
        throw new InvalidArgumentException("Argument invalide : doit être un entier, un fichier ou un répertoire");
    }
}
catch (Throwable $e) {
    echo "Error: " . $e->getMessage() . "\n";
    exit(1);
}
