module Parse
  ( parse,
  )
where

import Control.Applicative
import Data.Either
import Data.Functor
import RPN
import Text.Parsec hiding (many, parse, token)
import qualified Text.Parsec as Parsec (parse)
import Text.ParserCombinators.Parsec (GenParser)
import Prelude hiding (lex)

type Parser a = GenParser Char () a

literal :: Parser Item
literal = Operand . read <$> some digit

op :: Parser Item
op =
  choice
    [ try (string "**") $> Operator Pow,
      try (string ">>") $> Operator LShift,
      try (string "<<") $> Operator RShift,
      try (string "&&") $> Operator And,
      try (string "||") $> Operator Or,
      try (string "==") $> Operator Eq,
      try (string "!=") $> Operator Ne,
      try (string "<=") $> Operator Le,
      try (string ">=") $> Operator Ge,
      try (char '*') $> Operator Mul,
      try (char '/') $> Operator Div,
      try (char '%') $> Operator Rem,
      try (char '+') $> Operator Plus,
      try (char '-') $> Operator Minus,
      try (char '<') $> Operator Lt,
      try (char '>') $> Operator Gt,
      try (char '&') $> Operator BitAnd,
      try (char '^') $> Operator BitXor,
      char '|' $> Operator BitOr
    ]

parens :: Parser a -> Parser a
parens = between (spaces *> char '(' *> spaces) (spaces *> char ')')

tree :: Parser Tree
tree =
  spaces
    *> choice
      [ try (Item <$> literal <?> "literal"),
        try (Item <$> op <?> "operator"),
        SubTree <$> parens (many tree <?> "expression")
      ]

parse :: String -> Either ParseError [Tree]
parse = Parsec.parse (many (tree <* spaces) <* eof) ""
