module RPN
  ( Tree (..),
    Item (..),
    Op (..),
    toRevPolish,
    eval,
  )
where

import Data.Bits hiding (And)
import Data.List

type Precedence = Int

isLeftAssociative :: Op -> Bool
isLeftAssociative Pow = False
isLeftAssociative _ = True

isRightAssociative :: Op -> Bool
isRightAssociative = not . isLeftAssociative

data Op
  = Pow
  | Mul
  | Div
  | Rem
  | Plus
  | Minus
  | LShift
  | RShift
  | Lt
  | Le
  | Gt
  | Ge
  | Eq
  | Ne
  | BitAnd
  | BitXor
  | BitOr
  | And
  | Or

instance Eq Op where
  l == r = precedence l == precedence r

instance Ord Op where
  l <= r = precedence l <= precedence r

instance Show Op where
  show Pow = "**"
  show Mul = "*"
  show Div = "/"
  show Rem = "%"
  show Plus = "+"
  show Minus = "-"
  show LShift = "<<"
  show RShift = ">>"
  show Lt = "<"
  show Le = "<="
  show Gt = ">"
  show Ge = ">="
  show Eq = "=="
  show Ne = "!="
  show BitAnd = "&"
  show BitXor = "^"
  show BitOr = "|"
  show And = "&&"
  show Or = "||"

precedence :: Op -> Precedence
precedence Pow = 11
precedence Mul = 10
precedence Div = 10
precedence Rem = 10
precedence Plus = 9
precedence Minus = 9
precedence LShift = 8
precedence RShift = 8
precedence Lt = 7
precedence Le = 7
precedence Gt = 7
precedence Ge = 7
precedence Eq = 6
precedence Ne = 6
precedence BitAnd = 5
precedence BitXor = 4
precedence BitOr = 3
precedence And = 2
precedence Or = 1

data Item
  = Operand Int
  | Operator Op

instance Show Item where
  show (Operand n) = show n
  show (Operator op) = show op

data Tree
  = SubTree [Tree]
  | Item Item
  deriving (Show)

type Stack a = [a]

push :: Stack a -> a -> Stack a
push s i = i : s

pop :: Stack a -> Maybe (a, Stack a)
pop = uncons

toRevPolish' :: Stack Op -> [Tree] -> [Tree]
toRevPolish' s [] = map (Item . Operator) s
toRevPolish' s ((SubTree t) : ts) = SubTree (toRevPolish' [] t) : toRevPolish' s ts
toRevPolish' s ((Item (Operand n)) : ts) = Item (Operand n) : toRevPolish' s ts
toRevPolish' s ((Item (Operator op)) : ts) =
  case pop s of
    Nothing -> toRevPolish' (push s op) ts
    Just (topOp, _) ->
      let (h, t) = span (\x -> x >= op || (x == op && isLeftAssociative op)) s
       in map (Item . Operator) h ++ toRevPolish' (push t op) ts

toLinear :: [Tree] -> [Item]
toLinear ((Item x) : xs) = x : toLinear xs
toLinear ((SubTree x) : xs) = toLinear x ++ toLinear xs
toLinear [] = []

toRevPolish :: [Tree] -> [Item]
toRevPolish = toLinear . toRevPolish' []

evalOp :: Op -> Int -> Int -> Int
evalOp op l r = case op of
  Pow -> l ^ r
  Mul -> l * r
  Div -> l `div` r
  Rem -> l `mod` r
  Plus -> l + r
  Minus -> l - r
  LShift -> l `unsafeShiftL` r
  RShift -> l `unsafeShiftR` r
  Lt -> boolToInt (l < r)
  Le -> boolToInt (l <= r)
  Gt -> boolToInt (l > r)
  Ge -> boolToInt (l >= r)
  Eq -> boolToInt (l == r)
  Ne -> boolToInt (l /= r)
  BitAnd -> l .&. r
  BitXor -> l `xor` r
  BitOr -> l .|. r
  And -> boolToInt $ intToBool l && intToBool r
  Or -> boolToInt $ intToBool l || intToBool r
  where
    boolToInt True = 1
    boolToInt False = 0
    intToBool x = x /= 0

eval' :: Stack Int -> [Item] -> Stack Int
eval' s [] = s
eval' s ((Operand n) : xs) = eval' (push s n) xs
eval' (r : l : s) ((Operator op) : xs) = eval' (push s (evalOp op l r)) xs
eval' _ _ = error "invalid stack state, parser did something wrong"

eval :: [Item] -> Int
eval [] = undefined
eval xs = case eval' [] xs of
  [x] -> x
  _ -> error "something went terribly wrong"
