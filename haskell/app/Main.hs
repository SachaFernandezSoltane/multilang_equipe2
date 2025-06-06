import System.Environment (getArgs)
import System.Directory (doesDirectoryExist, listDirectory, doesFileExist)
import System.FilePath ((</>), takeExtension)
import System.Random (randomRIO)
import Control.Monad (forM_)
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
        chosen <- mapM (\x -> do b <- randomRIO (False, True); return (if b then x else 0)) (drop 1 base)
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
    go (x:xs) remT acc
        | remT < 0  = []
        | otherwise =
            let without = go xs remT acc
                with    = go xs (remT - x) (x:acc)
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

-- Process single file
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
