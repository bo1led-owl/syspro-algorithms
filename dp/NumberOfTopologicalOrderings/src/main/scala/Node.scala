import scala.collection.mutable.ArrayBuffer

class Node private (val children: ArrayBuffer[Node], val size: Int) {
  var topoligicalOrderings = BigInt(1)

  def this(nodes: Node*) = this(
    ArrayBuffer(nodes*),
    nodes.iterator.map(_.size).sum + 1,
  )
  def flatten: ArrayBuffer[Node] = this +: (children flatMap (_.flatten))
  def nChildren: Int             = children.length
}
