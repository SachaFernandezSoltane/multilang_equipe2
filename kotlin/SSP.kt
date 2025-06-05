import java.io.File
import java.io.FileNotFoundException
import java.util.Random
import java.util.Scanner

class SSP {
    private var target: Long = 0L
    private var original: LongArray
    val solutions = mutableListOf<MutableList<Long>>()

    constructor(n: Int) {
        require(n > 2) { "SSP size is too small or nonpositive" }
        val R = Random()
        target = 1L
        original = LongArray(n)
        original[0] = 1L
        for (i in 1 until n) {
            original[i] = (i + 1).toLong()
            if (R.nextBoolean()) target += original[i]
        }
    }

    @Throws(IllegalArgumentException::class, FileNotFoundException::class)
    constructor(filename: String) {
        val input = File(filename)
        require(input.exists()) { "Given path/file does not exist" }
        val scan = Scanner(input)
        val IAE = IllegalArgumentException("Error while parsing input file")

        if (!scan.hasNext()) throw IAE
        val size = scan.nextInt()
        if (size <= 0) throw IAE
        original = LongArray(size)

        if (!scan.hasNext()) throw IAE
        target = scan.nextLong()

        for (i in 0 until size) {
            if (!scan.hasNext()) throw IAE
            original[i] = scan.nextLong()
        }
        scan.close()
    }

    fun totalSum(): Long = original.sum()

    private fun bp(i: Int, partial: Long, total: Long, x: BooleanArray) {
        if (partial + total < target || partial > target) return

        if (partial == target) {
            val sol = mutableListOf<Long>()
            for (k in 0 until i) {
                if (x[k]) sol.add(original[k])
            }
            solutions.add(sol)
            return
        }

        if (i == original.size) return

        val newTotal = total - original[i]
        x[i] = false
        bp(i + 1, partial, newTotal, x)

        x[i] = true
        bp(i + 1, partial + original[i], newTotal, x)

        x[i] = false
    }

    fun bp() {
        val x = BooleanArray(original.size) { false }
        val total = totalSum()
        bp(0, 0L, total, x)
    }

    override fun toString(): String = "SSP(n = ${original.size}; target = $target)"
    fun showTarget(): String = "Target is $target"
    fun showIntegers(): String = "Original set = [${original.joinToString(",")}]"
}

// ===============================
// MAIN — avec gestion des arguments
// ===============================
fun main(args: Array<String>) {
    if (args.isEmpty()) {
        println("Usage: kotlin SSPKt <integer N | inputfile.txt | directory/>")
        return
    }

    val arg = args[0]

    // Si l'argument se termine par "/", on considère que c'est un dossier
    if (arg.endsWith("/")) {
        val dir = File(arg)
        if (!dir.exists() || !dir.isDirectory) {
            println("Le dossier '$arg' n'existe pas ou n'est pas un répertoire.")
            return
        }

        val files = dir.listFiles { f -> f.isFile && f.name.endsWith(".txt") }
        if (files == null || files.isEmpty()) {
            println("Aucun fichier .txt trouvé dans le dossier.")
            return
        }

        for (file in files) {
            println("\n=== Traitement du fichier: ${file.name} ===")
            try {
                runSSP(SSP(file.path))
            } catch (e: Exception) {
                println("Erreur dans ${file.name}: ${e.message}")
            }
        }
    }
    // Sinon, essayer de lire comme entier ou fichier simple
    else {
        val ssp: SSP = try {
            val n = arg.toInt()
            SSP(n)
        } catch (e: NumberFormatException) {
            SSP(arg)
        }

        runSSP(ssp)
    }
}



fun runSSP(ssp: SSP) {
    println(ssp)
    println(ssp.showIntegers())
    println(ssp.showTarget())

    print("Running bp ... ")
    val start = System.currentTimeMillis()
    ssp.bp()
    val end = System.currentTimeMillis()
    println("done!")

    if (ssp.solutions.size < 10)
        println(ssp.solutions)
    else
        println("bp found ${ssp.solutions.size} solutions")

    println("Elapsed time: ${end - start} ms")
}



