import scala.collection.mutable.ArrayBuffer
import scala.collection.Seq

import scala.util.boundary
import scala.util.boundary.break

private def isValidTopologicalOrdering(tree: Node)(nodes: Seq[Node]): Boolean =
  def findPath(needle: Node)(tree: Node): Option[Seq[Node]] =
    if tree == needle then Some(Nil)
    else
      tree.children.iterator map
        { child => findPath(needle)(child) map { tree +: _ } } find
        { _.isDefined } map
        { _.get }
  def doesNotGoBeforeParent(i: Int): Boolean =
    val parents = findPath(nodes(i))(tree).get
    parents forall nodes.iterator.take(i).contains
  (0 until nodes.length) forall doesNotGoBeforeParent

object Naive:
  def solve(tree: Node): BigInt = solve(tree, tree.flatten)
  def solve(tree: Node, nodes: ArrayBuffer[Node]): BigInt =
    nodes.permutations.filter(isValidTopologicalOrdering(tree)).length
