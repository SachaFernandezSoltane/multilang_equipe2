
package main

import (
    "bufio"
    "fmt"
    "math/rand"
    "os"
    "strconv"
    "strings"
    "time"
)

type SSP struct {
    target    int
    original  []int
    solutions [][]int
}

// Constructeur pour une instance aléatoire
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

// Constructeur depuis un fichier avec :
// - Ligne 1 : n (nombre d’éléments)
// - Ligne 2 : target
// - Ligne 3 : tous les éléments séparés par espace
func NewSSPFromFile(filename string) *SSP {
    file, err := os.Open(filename)
    if err != nil {
        panic("Unable to open file")
    }
    defer file.Close()

    scanner := bufio.NewScanner(file)

    // Lire la première ligne (size)
    if !scanner.Scan() {
        panic("Unexpected EOF reading size")
    }
    size, err := strconv.Atoi(scanner.Text())
    if err != nil {
        panic("Invalid size")
    }

    // Lire la deuxième ligne (target)
    if !scanner.Scan() {
        panic("Unexpected EOF reading target")
    }
    target, err := strconv.Atoi(scanner.Text())
    if err != nil {
        panic("Invalid target")
    }

    // Lire la troisième ligne (tous les entiers séparés par espaces)
    if !scanner.Scan() {
        panic("Unexpected EOF reading original numbers")
    }
    line := scanner.Text()
    parts := strings.Fields(line) // sépare par n'importe quel espace
    if len(parts) < size {
        panic("Not enough integers on third line")
    }

    original := make([]int, size)
    for i := 0; i < size; i++ {
        original[i], err = strconv.Atoi(parts[i])
        if err != nil {
            panic("Invalid integer in original set")
        }
    }

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

// Appels récursifs branch-and-prune
func (s *SSP) bpRecursive(i, partial, total int, x []bool) {
    if partial+total < s.target || partial > s.target {
        return
    }

    if partial == s.target {
        sol := []int{}
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

// Affiche les résultats
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
    fmt.Printf("elapsed time: %d ms\n", elapsed.Milliseconds())
}

func main() {
    rand.Seed(time.Now().UnixNano())

    if len(os.Args) != 2 {
        fmt.Println("Usage: go run ssp.go <size|filename>")
        return
    }

    arg := os.Args[1]
    var ssp *SSP

    if n, err := strconv.Atoi(arg); err == nil {
        // Si l'argument est un entier, on crée aléatoirement
        ssp = NewSSPRandom(n)
    } else {
        // Sinon, on lit le fichier au format:
        // ligne 1 : size
        // ligne 2 : target
        // ligne 3 : liste d'entiers séparés par espaces
        ssp = NewSSPFromFile(arg)
    }

    ssp.Print()
}
