import scala.collection.mutable.ArrayBuffer

@main def main: Unit =
  println("Polynomial:")
  for tree <- trees
  do println(Polynomial.solve(tree.flatten))
  println("Naive:")
  for tree <- trees
  do println(Naive.solve(tree))

val trees = ArrayBuffer(
  Node(),
  Node(Node()),
  Node(Node(Node(Node(Node(Node()))))),
  Node(Node(), Node()),
  Node(Node(), Node(), Node()),
  Node(Node(Node(), Node()), Node(Node(), Node()), Node()),
  Node(
    Node(),
    Node(Node(Node(), Node())),
    Node(Node(Node(Node()), Node())),
  ),
  Node(
    Node(Node(), Node(), Node(), Node()),
    Node(Node(), Node(), Node()),
  ),
  Node(
    Node(Node(Node()), Node(Node()), Node()),
    Node(Node(Node()), Node(), Node()),
  ),
  Node(
    Node(Node(), Node(Node()), Node(Node()), Node()),
    Node(Node(), Node(), Node()),
  ),
)
