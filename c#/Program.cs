using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

class SSP
{
    private long target;
    public long[] original;   // On rend public pour tri dans Main
    private List<List<long>> solutions = new List<List<long>>();

    // Constructeur aléatoire (taille uniquement)
    public SSP(int n)
    {
        if (n <= 0) throw new ArgumentException("SSP size cannot be nonpositive");
        if (n <= 2) throw new ArgumentException("SSP size is too small");

        original = new long[n];
        original[0] = 1;
        target = 1;
        Random rand = new Random();
        for (int i = 1; i < n; i++)
        {
            original[i] = i + 1;
            if (rand.Next(2) == 1) target += original[i];
        }
    }

    // Constructeur depuis fichier avec tri décroissant
    //   Ligne 1 : size
    //   Ligne 2 : target
    //   Ligne 3 : tous les entiers séparés par espaces
    public SSP(string filename)
    {
        if (!File.Exists(filename))
            throw new ArgumentException("File does not exist");

        var lines = File.ReadAllLines(filename);
        if (lines.Length < 3)
            throw new ArgumentException("Input file must have at least 3 lines");

        if (!int.TryParse(lines[0], out int size) || size <= 0)
            throw new ArgumentException("Invalid size on first line");

        if (!long.TryParse(lines[1], out target))
            throw new ArgumentException("Invalid target on second line");

        var parts = lines[2]
            .Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length < size)
            throw new ArgumentException("Not enough integers on third line");

        original = parts
            .Take(size)
            .Select(s =>
            {
                if (!long.TryParse(s, out var v))
                    throw new ArgumentException($"Invalid integer '{s}' on third line");
                return v;
            })
            .ToArray();

        // Tri décroissant pour optimiser la prune
        Array.Sort(original);
        Array.Reverse(original);
    }

    private long TotalSum()
    {
        long sum = 0;
        foreach (var v in original)
            sum += v;
        return sum;
    }

    private void BPRecursive(int i, long partial, long total, bool[] x)
    {
        if (partial + total < target || partial > target) return;

        if (partial == target)
        {
            var sol = new List<long>();
            for (int k = 0; k < i; k++)
                if (x[k])
                    sol.Add(original[k]);
            solutions.Add(sol);
            return;
        }
        if (i >= original.Length) return;

        total -= original[i];

        x[i] = false;
        BPRecursive(i + 1, partial, total, x);

        x[i] = true;
        BPRecursive(i + 1, partial + original[i], total, x);
        x[i] = false;
    }

    public void RunBP()
    {
        bool[] x = new bool[original.Length];
        long total = TotalSum();
        BPRecursive(0, 0, total, x);
    }

    public override string ToString()
    {
        return $"SSP(n = {original.Length}; target = {target})";
    }

    public string ShowTarget() => $"Target is {target}";

    public string ShowIntegers() => "Original set = [" + string.Join(", ", original) + "]";

    public void PrintSolutions()
    {
        if (solutions.Count < 10)
        {
            foreach (var sol in solutions)
                Console.WriteLine("[" + string.Join(", ", sol) + "]");
        }
        else
        {
            Console.WriteLine($"bp found {solutions.Count} solutions");
        }
    }

    public static void Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: dotnet run <size|filename|directory>");
            return;
        }
        var arg = args[0];

        // Cas entier → instance aléatoire
        if (int.TryParse(arg, out int n))
        {
            var s = new SSP(n);
            Console.WriteLine(s);
            Console.WriteLine(s.ShowIntegers());
            Console.WriteLine(s.ShowTarget());
            Console.WriteLine();
            Console.Write("Running bp... ");
            var start = DateTime.Now;
            s.RunBP();
            var end = DateTime.Now;
            Console.WriteLine("done!");
            s.PrintSolutions();
            Console.WriteLine($"elapsed time: {(end - start).TotalMilliseconds} ms");
            return;
        }

        // Cas fichier unique
        if (File.Exists(arg))
        {
            var s = new SSP(arg);
            Console.WriteLine(s);
            Console.WriteLine(s.ShowIntegers());
            Console.WriteLine(s.ShowTarget());
            Console.WriteLine();
            Console.Write("Running bp... ");
            var start = DateTime.Now;
            s.RunBP();
            var end = DateTime.Now;
            Console.WriteLine("done!");
            s.PrintSolutions();
            Console.WriteLine($"elapsed time: {(end - start).TotalMilliseconds} ms");
            return;
        }

        // Cas dossier : traiter tous les .txt
        if (Directory.Exists(arg))
        {
            foreach (var file in Directory.GetFiles(arg, "*.txt"))
            {
                Console.WriteLine($"=== Traitement de : {file} ===");
                var s = new SSP(file);
                Console.WriteLine(s);
                Console.WriteLine(s.ShowIntegers());
                Console.WriteLine(s.ShowTarget());
                Console.WriteLine();
                Console.Write("Running bp... ");
                var start = DateTime.Now;
                s.RunBP();
                var end = DateTime.Now;
                Console.WriteLine("done!");
                s.PrintSolutions();
                Console.WriteLine($"elapsed time: {(end - start).TotalMilliseconds} ms\n");
            }
            return;
        }

        Console.WriteLine("Invalid argument: must be an integer, a file or a directory");
    }
}

