import scala.math.Ordered.orderingToOrdered

import scala.collection.Seq
import scala.collection.IndexedSeq
import scala.collection.mutable.ArrayBuffer

extension [A, B, C](f: (A, B) => C) def flip: (B, A) => C = (b, a) => f(a, b)

extension [A](s: IndexedSeq[A])
  def upperBound(x: A)(using ord: Ordering[A]): Int = {
    var low  = 0
    var high = s.size

    while low < high do
      val mid = low + (high - low) / 2
      if s(mid) <= x then low = mid + 1
      else high = mid
    low
  }

object SegmentTree {
  def from(n: Int): SegmentTree =
    var nodes: List[SegmentTree] = Iterator
      .continually(0)
      .take(n)
      .zipWithIndex
      .map(SegmentTreeLeaf.apply.flip.tupled)
      .toList
    while nodes.size > 1 do
      nodes = nodes
        .grouped(2)
        .map {
          case x :: y :: Nil => SegmentTreeNode(x, y)
          case x :: Nil      => x
          case _             => ???
        }
        .toList
    nodes.head
}

sealed trait SegmentTree {
  def accumulated: Int
  def accumulated(l: Int, r: Int): Int

  def intersects(i: Int): Boolean = lBound <= i && i <= rBound
  def intersects(l: Int, r: Int): Boolean =
    (l <= lBound && rBound <= r) || intersects(l) || intersects(r)

  def revive(i: Int): SegmentTree

  def lBound: Int
  def rBound: Int
}

class SegmentTreeNode private (
    val accumulated: Int,
    val lBound: Int,
    val rBound: Int,
    lChild: SegmentTree,
    rChild: SegmentTree,
) extends SegmentTree {
  def this(lChild: SegmentTree, rChild: SegmentTree) =
    this(
      lChild.accumulated + rChild.accumulated,
      lChild.lBound,
      rChild.rBound,
      lChild,
      rChild,
    )

  override def accumulated(l: Int, r: Int): Int =
    if !intersects(l, r) then 0
    else if l <= lBound && rBound <= r then accumulated
    else lChild.accumulated(l, r) + rChild.accumulated(l, r)

  override def revive(i: Int): SegmentTree =
    if intersects(i) then SegmentTreeNode(lChild.revive(i), rChild.revive(i)) else this
}

class SegmentTreeLeaf(i: Int, val accumulated: Int) extends SegmentTree {
  override def revive(j: Int): SegmentTree =
    if intersects(j) then SegmentTreeLeaf(i, 1) else this

  override def accumulated(l: Int, r: Int): Int =
    if intersects(l, r) then accumulated else 0

  override def intersects(j: Int): Boolean         = i == j
  override def intersects(l: Int, r: Int): Boolean = l <= i && i <= r

  override def lBound: Int = i
  override def rBound: Int = i
}

class PersistentSegTree private (versions: ArrayBuffer[SegmentTree])
    extends PartialFunction[Int, SegmentTree] {
  def this(n: Int) = this(ArrayBuffer(SegmentTree.from(n)))

  override def apply(i: Int): SegmentTree   = versions(i)
  override def isDefinedAt(i: Int): Boolean = versions.isDefinedAt(i)

  def revive(i: Int): Unit = versions += versions.last.revive(i)
}

class ElementsGreaterThan[A] private (
    sortedItems: IndexedSeq[A],
    tree: PersistentSegTree,
)(using ord: Ordering[A]) {
  def this(items: IndexedSeq[A])(using ord: Ordering[A]) =
    this(items.sorted(using ord.reverse), PersistentSegTree(items.size))(using ord.reverse)
    sortedItems foreach (items.indexOf andThen tree.revive)

  def apply(l: Int, r: Int)(k: A): Int = {
    val v = sortedItems.upperBound(k)
    tree(v).accumulated(l, r)
  }
}

@main def main: Unit = {
  val items = ArrayBuffer(46, 11, 40, 8, 2, 42, 65, 10)
  val gte   = ElementsGreaterThan[Int](items)

  val tests: List[(Int, Int, Int)] =
    (0, 7, 0)
      :: (1, 4, 10)
      :: (2, 6, 40)
      :: (0, 5, 11)
      :: (4, 7, 30)
      :: Nil

  tests foreach { (l, r, k) =>
    val expected = items.slice(l, r + 1).count { _ >= k }
    val actual   = gte(l, r)(k)

    assert(expected == actual, s"\n\n\t(l, r, k) = ($l, $r, $k), expected $expected, got $actual\n")
  }
}
