import scala.collection.mutable.ArrayBuffer

private def factorial(n: BigInt): BigInt    = (BigInt(1) to n).product
private def f(n: BigInt, m: BigInt): BigInt = factorial(n + m) / (factorial(n) * factorial(m))

extension (node: Node)
  private def childrenTopologialOrderingsCombinations: BigInt =
    val n           = node.children.length
    var res: BigInt = 1
    var prefixSum   = 0
    for i <- 1 until n do
      prefixSum += node.children(i - 1).size
      res *= f(prefixSum, node.children(i).size)
    res

object Polynomial:
  def solve(nodes: ArrayBuffer[Node]): BigInt =
    for
      node <- nodes.reverse
      _    <- 0 until node.nChildren
    do
      node.topoligicalOrderings = node.children.iterator.map(_.topoligicalOrderings).product
        * node.childrenTopologialOrderingsCombinations
    nodes(0).topoligicalOrderings
