import scala.io.Source
import scala.util.{Random, Try}
import java.io.{File, FileNotFoundException}
import scala.collection.mutable.ListBuffer

class SSP {
  // Attributs
  private var target: Long = 0
  private var original: Array[Long] = Array()
  val solutions: ListBuffer[List[Long]] = ListBuffer()

  // Constructeur (instances faciles)
  def this(n: Int) = {
    this()
    if (n <= 0) throw new IllegalArgumentException("SSP size cannot be nonpositive")
    if (n <= 2) throw new IllegalArgumentException("SSP size is too small")
    val R = new Random()
    target = 1
    original = new Array[Long](n)
    original(0) = 1
    for (i <- 1 until n) {
      original(i) = i + 1
      if (R.nextBoolean()) target += original(i)
    }
  }

  // Constructeur (depuis fichier texte)
  def this(filename: String) = {
    this()
    if (filename == null) throw new IllegalArgumentException("Given path/file is null")
    val file = new File(filename)
    if (!file.exists()) throw new IllegalArgumentException("File does not exist")

    val lines = Source.fromFile(file).getLines().filter(_.nonEmpty).toList
    val tokens = lines.flatMap(_.split("\\s+")).map(_.trim)
    if (tokens.length < 2) throw new IllegalArgumentException("Error while parsing input file")

    val size = tokens.head.toInt
    if (size <= 0 || tokens.length < size + 2) throw new IllegalArgumentException("Error while parsing input file")
    original = tokens.slice(2, 2 + size).map(_.toLong).toArray
    target = tokens(1).toLong
  }

  // Somme totale
  def totalSum(): Long = original.sum

  // Méthode récursive bp
  private def bp(i: Int, partial: Long, total: Long, x: Array[Boolean]): Unit = {
    if (partial + total < target || partial > target) return

    if (partial == target) {
      val sol = (0 until i).filter(x(_)).map(original(_)).toList
      solutions += sol
      return
    }

    if (i == original.length) return
    val newTotal = total - original(i)

    x(i) = false
    bp(i + 1, partial, newTotal, x)

    x(i) = true
    bp(i + 1, partial + original(i), newTotal, x)
    x(i) = false
  }

  // Lancement de l’algorithme branch and prune
  def bp(): Unit = {
    val x = Array.fill(original.length)(false)
    val total = totalSum()
    bp(0, 0L, total, x)
  }

  override def toString: String = s"SSP(n = ${original.length}; target = $target)"

  def showTarget(): String = s"Target is $target"

  def showIntegers(): String = {
    s"Original set = [${original.mkString(",")}]"
  }
}

object SSPApp {
  def main(args: Array[String]): Unit = {
    if (args.isEmpty) {
      println("Usage: scala SSP <integer|filename|directory>")
      return
    }

    val arg = args(0)
    val maybeInt = Try(arg.toInt)

    maybeInt.toOption match {
      case Some(n) =>
        val ssp = new SSP(n)
        println(ssp)
        println(ssp.showIntegers())
        println(ssp.showTarget())
        println()

        print("Running bp ... ")
        val start = System.currentTimeMillis()
        ssp.bp()
        val end = System.currentTimeMillis()
        println("done!")
        if (ssp.solutions.size < 10)
          println(ssp.solutions)
        else
          println(s"bp found ${ssp.solutions.size} solutions")
        println(s"elapsed time ${end - start}ms")

      case None =>
        val f = new File(arg)
        if (!f.exists()) {
          println("Argument is not an integer or a valid file/directory: " + arg)
          return
        }

        if (f.isFile) {
          try {
            val ssp = new SSP(arg)
            println(ssp)
            println(ssp.showIntegers())
            println(ssp.showTarget())
            println()

            print("Running bp ... ")
            val start = System.currentTimeMillis()
            ssp.bp()
            val end = System.currentTimeMillis()
            println("done!")
            if (ssp.solutions.size < 10)
              println(ssp.solutions)
            else
              println(s"bp found ${ssp.solutions.size} solutions")
            println(s"elapsed time ${end - start}ms")
          } catch {
            case ex: Exception =>
              println(s"Erreur lors du traitement du fichier : ${ex.getMessage}")
          }

        } else if (f.isDirectory) {
          val files = f.listFiles()
          if (files == null) {
            println(s"Le répertoire est vide ou inaccessible: $arg")
            return
          }
          for (file <- files if file.isFile && file.getName.endsWith(".txt")) {
            println(s"Traitement du fichier : ${file.getName}")
            try {
              val sspFile = new SSP(file.getAbsolutePath)
              println(sspFile)
              println(sspFile.showIntegers())
              println(sspFile.showTarget())
              println()

              print("Running bp ... ")
              val start = System.currentTimeMillis()
              sspFile.bp()
              val end = System.currentTimeMillis()
              println("done!")
              if (sspFile.solutions.size < 10)
                println(sspFile.solutions)
              else
                println(s"bp found ${sspFile.solutions.size} solutions")
              println(s"elapsed time ${end - start}ms")
            } catch {
              case ex: Exception =>
                println(s"Erreur lors du traitement du fichier ${file.getName} : ${ex.getMessage}")
            }
          }
        } else {
          println("Argument is not a valid file or directory: " + arg)
        }
    }
  }
}
