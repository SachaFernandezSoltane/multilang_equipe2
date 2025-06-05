
/* Subset Sum in Java
 *
 * MJ
 */

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random;
import java.util.Scanner;
import java.util.List;
import java.util.ArrayList;
import java.util.LinkedList;

public class SSP
{
   // attributs
   private long target;
   private long[] original;
   private List<ArrayList<Long>> solutions;

   // constructor (easy instances)
   public SSP(int n) throws IllegalArgumentException
   {
      if (n <= 0) throw new IllegalArgumentException("SSP size cannot be nonpositive");
      if (n <= 2) throw new IllegalArgumentException("SSP size is too small");
      Random R = new Random();
      this.target = 1;
      this.original = new long[n];
      this.original[0] = 1;
      for (int i = 1; i < n; i++)
      {
         this.original[i] = i + 1;
         if (R.nextBoolean())  this.target = this.target + this.original[i];
      }
      // Tri décroissant pour améliorer la prune
      java.util.Arrays.sort(this.original);
      for (int i = 0; i < n / 2; i++) {
         long temp = this.original[i];
         this.original[i] = this.original[n - 1 - i];
         this.original[n - 1 - i] = temp;
      }
      solutions = new LinkedList<ArrayList<Long>> ();
   }

   // constructor (from text file)
   public SSP(String filename) throws IllegalArgumentException, FileNotFoundException
   {
      if (filename == null) throw new IllegalArgumentException("Given path/file is null");
      File input = new File(filename);
      if (!input.exists()) throw new IllegalArgumentException("Given path/file does not exist");
      Scanner scan = new Scanner(input);
      IllegalArgumentException IAE = new IllegalArgumentException("Error while parsing input file");

      // number of elements in the original set
      if (!scan.hasNext()) throw IAE;
      int size = scan.nextInt();
      if (size <= 0) throw IAE;
      this.original = new long[size];

      // target
      if (!scan.hasNext()) throw IAE;
      this.target = scan.nextLong();

      // the original set
      for (int i = 0; i < size; i++)
      {
         if (!scan.hasNext()) throw IAE;
         this.original[i] = scan.nextLong();
      }
      // Tri décroissant pour améliorer la prune
      java.util.Arrays.sort(this.original);
      int n = this.original.length;
      for (int i = 0; i < n / 2; i++) {
         long temp = this.original[i];
         this.original[i] = this.original[n - 1 - i];
         this.original[n - 1 - i] = temp;
      }
      scan.close();
      solutions = new LinkedList<ArrayList<Long>> ();
   }

   // computing the sum of all integers
   public long totalSum()
   {
      int n = this.original.length;
      long sum = this.original[0];
      for (int i = 1; i < n; i++)  sum = sum + this.original[i];
      return sum;
   }

   // bp algorithm recursive calls (private method)
   private static final int MAX_SOLUTIONS = 10000000;

   private void bp(int i,long partial,long total,boolean[] x)
   {
      // are we out of bounds yet?
      if (partial + total < this.target || partial > this.target)  return;

      // did we find a new solution already?
      if (partial == this.target)
      {
         if (this.solutions.size() >= MAX_SOLUTIONS) return;
         // we store the solution (ie the selected integers) in the list
         ArrayList<Long> sol = new ArrayList<Long> (this.original.length);
         for (int k = 0; k < i; k++)  if (x[k])  sol.add(this.original[k]);
         this.solutions.add(sol);
         return;
      }

      // did we use all integers already?
      if (i == this.original.length)  return;
      total = total - this.original[i];

      // recursive call without original[i]
      x[i] = false;
      this.bp(i + 1,partial,total,x);

      // recursive call with original[i]
      x[i] = true;
      partial = partial + this.original[i];
      this.bp(i + 1,partial,total,x);
      x[i] = false;
   }

   // branch-and-prune (bp) algorithm
   public void bp()
   {
      int n = this.original.length;
      boolean[] x = new boolean[n];
      for (int i = 0; i < n; i++)  x[i] = false;
      long total = this.totalSum();
      this.bp(0,0L,total,x);
   }

   @Override
   public String toString()
   {
      return "SSP(n = " + this.original.length + "; target = " + this.target + ")";
   }

   // showing the target
   public String showTarget()
   {
      return "Target is " + this.target;
   }

   // showing the integers in the original set
   public String showIntegers()
   {
      String s = "Original set = [";
      for (int i = 0; i < this.original.length; i++)
      {
         s = s + this.original[i];
         if (i + 1 < this.original.length)  s = s + ",";
      }
      return s + "]";
   }

   // main

   public static void main(String[] args) throws Exception
   {
      if (args.length == 0) {
         printUsage();
         return;
      }
      String arg = args[0];
      java.io.File f = new java.io.File(arg);
      if (isInteger(arg)) {
         SSP ssp = new SSP(Integer.parseInt(arg));
         processSSP(ssp);
      } else if (f.exists()) {
         if (f.isFile()) {
            SSP ssp = new SSP(arg);
            processSSP(ssp);
         } else if (f.isDirectory()) {
            processDirectory(f);
         } else {
            System.err.println("Argument is not a file or directory: " + arg);
         }
      } else {
         System.err.println("Argument is not an integer or a valid file/directory: " + arg);
      }
   }

   private static void printUsage() {
      System.out.println("Usage: java SSP <integer|filename|directory>");
   }

   private static boolean isInteger(String s) {
      try {
         Integer.parseInt(s);
         return true;
      } catch (NumberFormatException e) {
         return false;
      }
   }

   private static void processSSP(SSP ssp) {
      System.out.println(ssp);
      System.out.println(ssp.showIntegers());
      System.out.println(ssp.showTarget());
      System.out.println();

      System.out.print("Running bp ... ");
      long start = System.currentTimeMillis();
      ssp.bp();
      long end = System.currentTimeMillis();
      System.out.println("done!");
      if (ssp.solutions.size() < 10)
         System.out.println(ssp.solutions);
      else
         System.out.println("bp found " + ssp.solutions.size() + " solutions");
      System.out.println("elapsed time " + (end - start));
      System.out.println();
   }

   private static void processDirectory(java.io.File dir) {
      java.io.File[] files = dir.listFiles();
      if (files == null) {
         System.err.println("Le répertoire est vide ou inaccessible: " + dir.getPath());
         return;
      }
      for (java.io.File file : files) {
         if (file.isFile() && file.getName().endsWith(".txt")) {
            System.out.println("Traitement du fichier : " + file.getName());
            try {
               SSP ssp = new SSP(file.getPath());
               processSSP(ssp);
            } catch (Exception e) {
               System.err.println("Erreur lors du traitement du fichier " + file.getName() + " : " + e.getMessage());
            }
         }
      }
   }
}
