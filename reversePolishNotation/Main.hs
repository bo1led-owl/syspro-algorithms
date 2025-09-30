module Main where

import Control.Monad
import Parse
import RPN
import System.Console.Haskeline
import System.IO
import Prelude hiding (lex)

main :: IO ()
main = do
  runInputT defaultSettings loop
  where
    loop :: InputT IO ()
    loop = do
      minput <- getInputLine " > "
      case minput of
        Nothing -> return ()
        Just line -> do
          unless (null line) (handleLine line)
          loop

    handleLine :: String -> InputT IO ()
    handleLine line =
      case parse line of
        Left err -> outputStrLn $ show err
        Right tree ->
          let polish = toRevPolish tree
           in outputStrLn (" (" ++ (unwords . map show) polish ++ ") = " ++ show (eval polish))
