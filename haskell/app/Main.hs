import System.Environment (getArgs)
import System.Directory (doesDirectoryExist, listDirectory, doesFileExist)
import System.FilePath ((</>), takeExtension)
import System.Random (randomRIO)
import Control.Monad (forM_)
import Data.Time.Clock (getCurrentTime, diffUTCTime, NominalDiffTime)
import Text.Printf (printf)
import Data.List (intercalate, sortBy)
import Control.Exception (catch, IOException)
import Control.DeepSeq (deepseq)

-- Constants
maxSolutions :: Int
maxSolutions = 10000000

-- SSP data structure
data SSP = SSP { target :: Integer, original :: [Integer] }

-- Generate SSP with random target and descending sort
randomSSP :: Int -> IO SSP
randomSSP n
    | n <= 2    = error "SSP size is too small or nonpositive"
    | otherwise = do
        let base = [1..fromIntegral n]
        chosen <- mapM (\x -> do b <- randomRIO (False, True); return (if b then x else 0)) (drop 1 base)
        let totalTarget = 1 + sum chosen
        return $ SSP totalTarget (reverse base)  -- Sorted in descending order

-- Read SSP from file and sort descending
readSSP :: FilePath -> IO SSP
readSSP file = do
    contents <- readFile file
    let ws = words contents
    let parseError = error "Error while parsing input file"
    case ws of
        (szStr:tStr:rest) -> do
            let size = read szStr
            let tgt  = read tStr
            if length rest < size then parseError
            else return $ SSP tgt (reverse . sortBy compare $ take size $ map read rest)
        _ -> parseError

-- Backtracking with limit and pruning
subsetSum :: SSP -> [[Integer]]
subsetSum (SSP tgt nums) = take maxSolutions $ go nums tgt []
  where
    go [] 0 acc = [reverse acc]
    go [] _ _   = []
    go (x:xs) remT acc
        | remT < 0         = []
        | remT < sum xs    = with ++ without
        | otherwise        = with ++ without
      where
        without = go xs remT acc
        with    = go xs (remT - x) (x:acc)

-- Display SSP
showSSP :: SSP -> String
showSSP (SSP tgt nums) =
    "SSP(n = " ++ show (length nums) ++ "; target = " ++ show tgt ++ ")\n" ++
    "Original set = [" ++ intercalate "," (map show nums) ++ "]\n" ++
    "Target is " ++ show tgt

-- Convert duration to ms
printDuration :: NominalDiffTime -> IO ()
printDuration t = printf "Elapsed time: %.3f ms\n" (realToFrac t * 1000 :: Double)

-- Run SSP
runSSP :: SSP -> IO ()
runSSP ssp = do
    putStrLn $ showSSP ssp
    putStr "Running bp ... "
    start <- getCurrentTime
    let sols = subsetSum ssp
    sols `deepseq` return ()  -- force evaluation
    end <- getCurrentTime
    putStrLn "done!"
    if length sols < 10
        then print sols
        else putStrLn $ "bp found " ++ show (length sols) ++ " solutions"
    printDuration (diffUTCTime end start)

-- Process a single file
processFile :: FilePath -> IO ()
processFile file = do
    putStrLn $ "\n=== Traitement du fichier: " ++ file ++ " ==="
    result <- readSSP file `catch` (\e -> print (e :: IOException) >> return (SSP 0 []))
    runSSP result

-- Main logic
main :: IO ()
main = do
    args <- getArgs
    if null args
        then putStrLn "Usage: stack exec ssp-project \"<N | inputfile.txt | directory/>\""
        else do
            let arg = head args
            dirExists <- doesDirectoryExist arg
            fileExists <- doesFileExist arg

            if dirExists
                then do
                    files <- listDirectory arg
                    let txtFiles = filter (\f -> takeExtension f == ".txt") files
                    if null txtFiles
                        then putStrLn "Aucun fichier .txt trouvé dans le dossier."
                        else forM_ txtFiles $ \file -> processFile (arg </> file)

            else if fileExists
                then processFile arg

            else case reads arg :: [(Int, String)] of
                [(n, "")] -> randomSSP n >>= runSSP
                _ -> putStrLn "Fichier ou dossier introuvable, ou argument invalide."
