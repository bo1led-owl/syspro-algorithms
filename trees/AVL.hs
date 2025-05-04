module AVL
  ( AVL,
    singleton,
    empty,
    isEmpty,
    contains,
    insert,
    from,
    toList,
    lookup,
    minimum,
    maximum,
    remove,
    size,
    index,
    takeLT,
    takeGT,
    takeLE,
    takeGE,
    dropGE,
    dropGT,
    dropLE,
    dropLT,
    update,
  )
where

import Data.Maybe (isJust)
import Prelude hiding (lookup, maximum, minimum)

data AVL a
  = Empty
  | Node (AVL a) a (AVL a) Int Int
  deriving (Show)

height :: AVL a -> Int
height Empty = 0
height (Node _ _ _ h _) = h

size :: AVL a -> Int
size Empty = 0
size (Node _ _ _ _ s) = s

nodeFrom :: AVL a -> a -> AVL a -> AVL a
nodeFrom l x r = Node l x r (max (height l) (height r) + 1) (size l + size r + 1)

balanceFactor :: AVL a -> Int
balanceFactor Empty = 0
balanceFactor (Node l _ r _ _) = height r - height l

simpleLeftRotation :: AVL a -> AVL a
simpleLeftRotation Empty = Empty
simpleLeftRotation (Node l x (Node rl y rr _ _) _ _) = nodeFrom (nodeFrom l x rl) y rr
simpleLeftRotation (Node _ _ Empty _ _) = undefined

simpleRightRotation :: AVL a -> AVL a
simpleRightRotation Empty = Empty
simpleRightRotation (Node (Node ll y lr _ _) x r _ _) = nodeFrom ll y (nodeFrom lr x r)
simpleRightRotation (Node Empty _ _ _ _) = undefined

doubleLeftRotation :: AVL a -> AVL a
doubleLeftRotation Empty = Empty
doubleLeftRotation (Node l x r _ _) = simpleLeftRotation (nodeFrom l x (simpleRightRotation r))

doubleRightRotation :: AVL a -> AVL a
doubleRightRotation Empty = Empty
doubleRightRotation (Node l x r _ _) = simpleRightRotation (nodeFrom (simpleLeftRotation l) x r)

rotateIfNeeded :: AVL a -> AVL a
rotateIfNeeded Empty = Empty
rotateIfNeeded node
  | b > 1 && balanceFactor r < 0 = doubleLeftRotation node
  | b > 1 = simpleLeftRotation node
  | b < -1 && balanceFactor l > 0 = doubleRightRotation node
  | b < -1 = simpleRightRotation node
  | otherwise = node
  where
    (Node l _ r _ _) = node
    b = balanceFactor node

instance Functor AVL where
  fmap _ Empty = Empty
  fmap f (Node l v r h s) = Node (fmap f l) (f v) (fmap f r) h s

instance Foldable AVL where
  foldMap _ Empty = mempty
  foldMap f (Node l v r _ _) = foldMap f l `mappend` f v `mappend` foldMap f r

instance Traversable AVL where
  traverse _ Empty = pure Empty
  traverse f (Node l v r h s) = Node <$> traverse f l <*> f v <*> traverse f r <*> pure h <*> pure s

empty :: AVL a
empty = Empty

isEmpty :: AVL a -> Bool
isEmpty Empty = True
isEmpty _ = False

singleton :: a -> AVL a
singleton x = Node Empty x Empty 1 1

from :: (Foldable t) => (a -> a -> Ordering) -> t a -> AVL a
from cmp = foldr (\i t -> AVL.insert (i `cmp`) i t) AVL.empty

toList :: AVL a -> [a]
toList = foldr (:) []

minimum :: AVL a -> a
minimum Empty = error "tree is empty"
minimum (Node Empty x _ _ _) = x
minimum (Node l _ _ _ _) = minimum l

maximum :: AVL a -> a
maximum Empty = error "tree is empty"
maximum (Node _ x Empty _ _) = x
maximum (Node _ _ r _ _) = maximum r

contains :: (a -> Ordering) -> AVL a -> Bool
contains cmp t = isJust $ lookup cmp t

lookup :: (a -> Ordering) -> AVL a -> Maybe a
lookup _ Empty = Nothing
lookup cmp (Node l v r _ _) = case cmp v of
  LT -> lookup cmp l
  EQ -> Just v
  GT -> lookup cmp r

takeLT :: (a -> Ordering) -> AVL a -> AVL a
takeLT _ Empty = Empty
takeLT cmp node =
  let (Node l x r _ _) = node
   in case cmp x of
        GT -> nodeFrom l x (takeLT cmp r)
        _ -> takeLT cmp l

takeLE :: (a -> Ordering) -> AVL a -> AVL a
takeLE _ Empty = Empty
takeLE cmp node =
  let (Node l x r _ _) = node
   in case cmp x of
        LT -> takeLE cmp l
        EQ -> nodeFrom (takeLE cmp l) x (takeLE cmp r)
        GT -> nodeFrom l x (takeLE cmp r)

takeGT :: (a -> Ordering) -> AVL a -> AVL a
takeGT _ Empty = Empty
takeGT cmp node =
  let (Node l x r _ _) = node
   in case cmp x of
        LT -> nodeFrom (takeGT cmp l) x r
        _ -> takeGT cmp r

takeGE :: (a -> Ordering) -> AVL a -> AVL a
takeGE _ Empty = Empty
takeGE cmp node =
  let (Node l x r _ _) = node
   in case cmp x of
        LT -> nodeFrom (takeGE cmp l) x r
        EQ -> nodeFrom (takeGE cmp l) x (takeGE cmp r)
        GT -> takeGE cmp r

dropLT :: (a -> Ordering) -> AVL a -> AVL a
dropLT = takeGE

dropLE :: (a -> Ordering) -> AVL a -> AVL a
dropLE = takeGT

dropGT :: (a -> Ordering) -> AVL a -> AVL a
dropGT = takeLE

dropGE :: (a -> Ordering) -> AVL a -> AVL a
dropGE = takeLT

index :: (a -> Ordering) -> AVL a -> Int
index _ Empty = error "tree is empty"
index cmp (Node l x r _ _) =
  case cmp x of
    LT -> index cmp l
    EQ -> size l
    GT -> size l + 1 + index cmp r

insert :: (a -> Ordering) -> a -> AVL a -> AVL a
insert _ x Empty = singleton x
insert cmp x node =
  let (Node l y r _ _) = node
   in case cmp y of
        LT -> rotateIfNeeded $ nodeFrom (insert cmp x l) y r
        EQ -> node
        GT -> rotateIfNeeded $ nodeFrom l y (insert cmp x r)

update :: (a -> Ordering) -> a -> AVL a -> AVL a
update _ x Empty = singleton x
update cmp x node =
  let (Node l y r h s) = node
   in case cmp y of
        LT -> rotateIfNeeded $ nodeFrom (update cmp x l) y r
        EQ -> Node l x r h s
        GT -> rotateIfNeeded $ nodeFrom l y (update cmp x r)

remove :: (a -> Ordering) -> AVL a -> AVL a
remove _ Empty = Empty
remove cmp node =
  let (Node l y r _ _) = node
   in case cmp y of
        LT -> rotateIfNeeded $ nodeFrom (remove cmp l) y r
        EQ -> helper node
        GT -> rotateIfNeeded $ nodeFrom l y (remove cmp r)
  where
    helper :: AVL a -> AVL a
    helper Empty = Empty
    helper (Node Empty _ Empty _ _) = Empty
    helper (Node Empty _ n _ _) = n
    helper (Node n _ Empty _ _) = n
    helper (Node l _ r _ _) = nodeFrom l (minimum r) (removeMinimum r)

    removeMinimum Empty = Empty
    removeMinimum (Node Empty _ r _ _) = r
    removeMinimum (Node l x r _ _) = rotateIfNeeded $ nodeFrom (removeMinimum l) x r
