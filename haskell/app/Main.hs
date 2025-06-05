import System.Environment (getArgs)
import System.Directory (doesDirectoryExist, listDirectory, doesFileExist)
import System.FilePath ((</>), takeExtension)
import System.Random (randomRIO)
import Control.Monad (filterM, forM_, when)
import Data.Time.Clock (getCurrentTime, diffUTCTime)
import Data.List (intercalate)
import Control.Exception (catch, IOException)

-- SSP data structure
data SSP = SSP { target :: Integer, original :: [Integer] }

-- Generate SSP with random target
randomSSP :: Int -> IO SSP
randomSSP n
    | n <= 2    = error "SSP size is too small or nonpositive"
    | otherwise = do
        let base = [1..fromIntegral n]
        chosen <- mapM (\x -> do b <- randomRIO (False, True); return (if b then x else 0)) (tail base)
        let totalTarget = 1 + sum chosen
        return $ SSP totalTarget base

-- Read SSP from file
readSSP :: FilePath -> IO SSP
readSSP file = do
    contents <- readFile file
    let ws = words contents
    let parseError = error "Error while parsing input file"
    case ws of
        (szStr:tStr:rest) -> do
            let size = read szStr
            let tgt  = read tStr
            if length rest < size then parseError else return $ SSP tgt (take size $ map read rest)
        _ -> parseError

-- Backtracking
subsetSum :: SSP -> [[Integer]]
subsetSum (SSP tgt nums) = go nums tgt []
  where
    go [] 0 acc = [reverse acc]
    go [] _ _   = []
    go (x:xs) rem acc
        | rem < 0  = []
        | otherwise =
            let without = go xs rem acc
                with    = go xs (rem - x) (x:acc)
            in with ++ without

-- Display SSP
showSSP :: SSP -> String
showSSP (SSP tgt nums) =
    "SSP(n = " ++ show (length nums) ++ "; target = " ++ show tgt ++ ")\n" ++
    "Original set = [" ++ intercalate "," (map show nums) ++ "]\n" ++
    "Target is " ++ show tgt

-- Run SSP
runSSP :: SSP -> IO ()
runSSP ssp = do
    putStrLn $ showSSP ssp
    putStr "Running bp ... "
    start <- getCurrentTime
    let sols = subsetSum ssp
    end <- getCurrentTime
    putStrLn "done!"
    if length sols < 10
        then print sols
        else putStrLn $ "bp found " ++ show (length sols) ++ " solutions"
    putStrLn $ "Elapsed time: " ++ show (diffUTCTime end start)

-- Main logic
main :: IO ()
main = do
    args <- getArgs
    if null args
        then putStrLn "Usage: runhaskell SSP.hs <N | inputfile.txt | directory/>"
        else do
            let arg = head args
            if last arg == '/'
                then do
                    exists <- doesDirectoryExist arg
                    if not exists
                        then putStrLn $ "Le dossier '" ++ arg ++ "' n'existe pas ou n'est pas un répertoire."
                        else do
                            files <- listDirectory arg
                            let txtFiles = filter (\f -> takeExtension f == ".txt") files
                            fullPaths <- filterM (doesFileExist . (arg </>)) txtFiles
                            if null fullPaths
                                then putStrLn "Aucun fichier .txt trouvé dans le dossier."
                                else forM_ fullPaths $ \file -> do
                                    putStrLn $ "\n=== Traitement du fichier: " ++ file ++ " ==="
                                    result <- readSSP (arg </> file)
                                        `catch` (\e -> print (e :: IOError) >> return (SSP 0 []))
                                    runSSP result
                else do
                    let maybeInt = reads arg :: [(Int, String)]
                    ssp <- if not (null maybeInt)
                        then randomSSP (fst (head maybeInt))
                        else readSSP arg
                    runSSP ssp
