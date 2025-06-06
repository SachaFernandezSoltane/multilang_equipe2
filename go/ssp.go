package main

import (
    "bufio"
    "fmt"
    "math/rand"
    "os"
    "path/filepath"
    "sort"
    "strconv"
    "strings"
    "time"
)

type SSP struct {
    target    int
    original  []int
    solutions [][]int
}

// Constructeur pour instance aléatoire (taille uniquement)
func NewSSPRandom(n int) *SSP {
    if n <= 2 {
        panic("SSP size must be > 2")
    }
    original := make([]int, n)
    for i := 0; i < n; i++ {
        original[i] = i + 1
    }
    target := 1
    for i := 1; i < n; i++ {
        if rand.Intn(2) == 1 {
            target += original[i]
        }
    }
    return &SSP{target: target, original: original}
}

// Constructeur depuis fichier avec tri décroissant
// Format attendu :
//   Ligne 1 : n (nombre d’éléments)
//   Ligne 2 : target
//   Ligne 3 : tous les éléments séparés par espaces
func NewSSPFromFile(filename string) *SSP {
    file, err := os.Open(filename)
    if err != nil {
        panic("Unable to open file: " + err.Error())
    }
    defer file.Close()

    scanner := bufio.NewScanner(file)

    // Ligne 1 : size
    if !scanner.Scan() {
        panic("Unexpected EOF reading size")
    }
    size, err := strconv.Atoi(strings.TrimSpace(scanner.Text()))
    if err != nil {
        panic("Invalid size: " + err.Error())
    }

    // Ligne 2 : target
    if !scanner.Scan() {
        panic("Unexpected EOF reading target")
    }
    target, err := strconv.Atoi(strings.TrimSpace(scanner.Text()))
    if err != nil {
        panic("Invalid target: " + err.Error())
    }

    // Ligne 3 : tous les entiers séparés par espaces
    if !scanner.Scan() {
        panic("Unexpected EOF reading original numbers")
    }
    parts := strings.Fields(scanner.Text())
    if len(parts) < size {
        panic("Not enough integers on third line")
    }

    original := make([]int, size)
    for i := 0; i < size; i++ {
        original[i], err = strconv.Atoi(parts[i])
        if err != nil {
            panic("Invalid integer in original set: " + err.Error())
        }
    }

    // Tri décroissant pour améliorer la prune
    sort.Slice(original, func(i, j int) bool {
        return original[i] > original[j]
    })

    return &SSP{target: target, original: original}
}

// Somme totale de tous les éléments
func (s *SSP) totalSum() int {
    sum := 0
    for _, v := range s.original {
        sum += v
    }
    return sum
}

// Appel récursif branch-and-prune
func (s *SSP) bpRecursive(i, partial, total int, x []bool) {
    if partial+total < s.target || partial > s.target {
        return
    }
    if partial == s.target {
        sol := make([]int, 0)
        for idx, used := range x {
            if used {
                sol = append(sol, s.original[idx])
            }
        }
        s.solutions = append(s.solutions, sol)
        return
    }
    if i >= len(s.original) {
        return
    }

    total -= s.original[i]

    x[i] = false
    s.bpRecursive(i+1, partial, total, x)

    x[i] = true
    s.bpRecursive(i+1, partial+s.original[i], total, x)

    x[i] = false
}

// Lance l’algorithme branch-and-prune
func (s *SSP) RunBP() {
    x := make([]bool, len(s.original))
    s.bpRecursive(0, 0, s.totalSum(), x)
}

// Affiche les résultats (instance + solutions + temps)
func (s *SSP) Print() {
    fmt.Printf("SSP(n = %d; target = %d)\n", len(s.original), s.target)
    fmt.Printf("Original set = %v\n", s.original)
    fmt.Printf("Target is %d\n\n", s.target)

    fmt.Println("Running bp...")
    start := time.Now()
    s.RunBP()
    fmt.Println("done!")

    if len(s.solutions) < 10 {
        for _, sol := range s.solutions {
            fmt.Println(sol)
        }
    } else {
        fmt.Printf("bp found %d solutions\n", len(s.solutions))
    }
    elapsed := time.Since(start)
    fmt.Printf("elapsed time: %d ms\n\n", elapsed.Milliseconds())
}

func main() {
    rand.Seed(time.Now().UnixNano())

    if len(os.Args) != 2 {
        fmt.Println("Usage: go run ssp.go <size|filename|directory>")
        return
    }
    arg := os.Args[1]

    // 1) Si c'est un entier, on crée une instance aléatoire
    if n, err := strconv.Atoi(arg); err == nil {
        s := NewSSPRandom(n)
        s.Print()
        return
    }

    // 2) Sinon, on essaie d'obtenir des infos sur le fichier/répertoire
    info, err := os.Stat(arg)
    if err != nil {
        fmt.Println("Invalid argument:", err)
        return
    }

    // 3) Si c'est un fichier régulier, on lit et on trie, puis on exécute
    if info.Mode().IsRegular() {
        s := NewSSPFromFile(arg)
        s.Print()
        return
    }

    // 4) Si c'est un répertoire, on parcourt tous les .txt
    if info.Mode().IsDir() {
        entries, _ := os.ReadDir(arg)
        for _, entry := range entries {
            if !entry.IsDir() && strings.HasSuffix(entry.Name(), ".txt") {
                full := filepath.Join(arg, entry.Name())
                fmt.Printf("=== Traitement de : %s ===\n", full)
                s := NewSSPFromFile(full)
                s.Print()
            }
        }
        return
    }

    fmt.Println("Invalid argument: must be an integer, a file or a directory")
}
