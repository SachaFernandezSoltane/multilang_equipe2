/*
 * Subset Sum Problem (SSP) en C++
 * MJ
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <random>
#include <stdexcept>
#include <chrono>
#include <algorithm>

class SSP {
private:
    long long target;
    std::vector<long long> original;
    std::list<std::vector<long long>> solutions;

    // Appel récursif de l'algorithme branch-and-prune
    void bp(int i, long long partial, long long total, std::vector<bool>& x) {
        if (partial + total < target || partial > target) return;

        if (partial == target) {
            if (solutions.size() >= 10000000) return;
            std::vector<long long> sol;
            for (int k = 0; k < i; ++k) {
                if (x[k]) sol.push_back(original[k]);
            }
            solutions.push_back(sol);
            return;
        }

        if (i == (int)original.size()) return;

        total -= original[i];

        // Appel récursif sans original[i]
        x[i] = false;
        bp(i + 1, partial, total, x);

        // Appel récursif avec original[i]
        x[i] = true;
        partial += original[i];
        bp(i + 1, partial, total, x);
        x[i] = false;
    }

public:
    // Constructeur pour instances faciles (taille n)
    SSP(int n) {
        if (n <= 0) throw std::invalid_argument("SSP size cannot be nonpositive");
        if (n <= 2) throw std::invalid_argument("SSP size is too small");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution d(0.5);

        target = 1;
        original.resize(n);
        original[0] = 1;
        for (int i = 1; i < n; ++i) {
            original[i] = i + 1;
            if (d(gen)) target += original[i];
        }
        // Tri décroissant pour améliorer la prune
        std::sort(original.begin(), original.end(), std::greater<long long>());
    }

    // Constructeur à partir d'un fichier texte
    SSP(const std::string& filename) {
        std::ifstream input(filename);
        if (!input.is_open()) throw std::invalid_argument("Given path/file does not exist");

        int size;
        if (!(input >> size)) throw std::invalid_argument("Error while parsing input file");
        if (size <= 0) throw std::invalid_argument("Error while parsing input file");

        original.resize(size);

        if (!(input >> target)) throw std::invalid_argument("Error while parsing input file");

        for (int i = 0; i < size; ++i) {
            if (!(input >> original[i])) throw std::invalid_argument("Error while parsing input file");
        }
        // Tri décroissant pour améliorer la prune
        std::sort(original.begin(), original.end(), std::greater<long long>());
        input.close();
    }

    // Somme totale des entiers
    long long totalSum() const {
        long long sum = 0;
        for (auto val : original) sum += val;
        return sum;
    }

    // Algorithme branch-and-prune
    void bp() {
        int n = (int)original.size();
        std::vector<bool> x(n, false);
        long long total = totalSum();
        bp(0, 0LL, total, x);
    }

    // Affichage de l'objet SSP
    std::string toString() const {
        return "SSP(n = " + std::to_string(original.size()) + "; target = " + std::to_string(target) + ")";
    }

    // Affichage de la cible
    std::string showTarget() const {
        return "Target is " + std::to_string(target);
    }

    // Affichage des entiers originaux
    std::string showIntegers() const {
        std::string s = "Original set = [";
        for (size_t i = 0; i < original.size(); ++i) {
            s += std::to_string(original[i]);
            if (i + 1 < original.size()) s += ",";
        }
        s += "]";
        return s;
    }

    // Affichage des solutions (optionnel)
    void printSolutions() const {
        if (solutions.size() < 10) {
            for (const auto& sol : solutions) {
                std::cout << "[";
                for (size_t i = 0; i < sol.size(); ++i) {
                    std::cout << sol[i];
                    if (i + 1 < sol.size()) std::cout << ",";
                }
                std::cout << "]" << std::endl;
            }
        } else {
            std::cout << "bp found " << solutions.size() << " solutions" << std::endl;
        }
    }
};

#include <filesystem>

void processDirectory(const std::string& dirPath) {
    namespace fs = std::filesystem;
    if (!fs::is_directory(dirPath)) {
        std::cerr << "Le répertoire est vide ou inaccessible: " << dirPath << std::endl;
        return;
    }
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::cout << "Traitement du fichier : " << entry.path().filename().string() << std::endl;
            try {
                SSP ssp(entry.path().string());
                std::cout << ssp.toString() << std::endl;
                std::cout << ssp.showIntegers() << std::endl;
                std::cout << ssp.showTarget() << std::endl;
                std::cout << std::endl;

                std::cout << "Running bp ... ";
                auto start = std::chrono::high_resolution_clock::now();
                ssp.bp();
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << "done!" << std::endl;

                ssp.printSolutions();

                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                std::cout << "elapsed time " << elapsed.count() << " ms" << std::endl;
                std::cout << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Erreur lors du traitement du fichier " << entry.path().filename().string() << " : " << e.what() << std::endl;
            }
        }
    }
}

bool isInteger(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: SSP <integer|filename|directory>" << std::endl;
        return 0;
    }
    std::string arg = argv[1];
    try {
        if (isInteger(arg)) {
            int n = std::stoi(arg);
            SSP ssp(n);
            std::cout << ssp.toString() << std::endl;
            std::cout << ssp.showIntegers() << std::endl;
            std::cout << ssp.showTarget() << std::endl;
            std::cout << std::endl;

            std::cout << "Running bp ... ";
            auto start = std::chrono::high_resolution_clock::now();
            ssp.bp();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "done!" << std::endl;

            ssp.printSolutions();

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "elapsed time " << elapsed.count() << " ms" << std::endl;
            std::cout << std::endl;
        } else {
            namespace fs = std::filesystem;
            fs::path p(arg);
            if (fs::exists(p)) {
                if (fs::is_regular_file(p)) {
                    SSP ssp(arg);
                    std::cout << ssp.toString() << std::endl;
                    std::cout << ssp.showIntegers() << std::endl;
                    std::cout << ssp.showTarget() << std::endl;
                    std::cout << std::endl;

                    std::cout << "Running bp ... ";
                    auto start = std::chrono::high_resolution_clock::now();
                    ssp.bp();
                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << "done!" << std::endl;

                    ssp.printSolutions();

                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    std::cout << "elapsed time " << elapsed.count() << " ms" << std::endl;
                    std::cout << std::endl;
                } else if (fs::is_directory(p)) {
                    processDirectory(arg);
                } else {
                    std::cerr << "Argument is not a file or directory: " << arg << std::endl;
                }
            } else {
                std::cerr << "Argument is not an integer or a valid file/directory: " << arg << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
