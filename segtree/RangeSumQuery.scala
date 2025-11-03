// https://leetcode.com/problems/range-sum-query-mutable/submissions/1812416952/

trait Semigroup[A]:
  def add(a: A, b: A): A

trait Monoid[A] extends Semigroup[A]:
  def zero: A

given Monoid[Int] with
  def add(a: Int, b: Int): Int = a + b
  def zero: Int                = 0

object SegmentTree {
  def from[A](items: IterableOnce[A])(using m: Monoid[A]): SegmentTree[A] =
    var nodes: List[SegmentTree[A]] = items.iterator.zipWithIndex.map { (x, i) =>
      SegmentTreeLeaf[A](i, x)
    }.toList
    while nodes.size > 1 do
      nodes = nodes
        .grouped(2)
        .map {
          case x :: y :: Nil => SegmentTreeNode[A](x, y)
          case x :: Nil      => x
          case _ => ???
        }
        .toList
    nodes.head
}

sealed trait SegmentTree[A](using m: Monoid[A]) {
  def accumulated: A
  def accumulated(l: Int, r: Int): A

  def intersects(i: Int): Boolean
  def intersects(l: Int, r: Int): Boolean

  def update(i: Int, x: A): SegmentTree[A]

  def lBound: Int
  def rBound: Int
}

class SegmentTreeNode[A] private (
    private var acc: A,
    val lBound: Int,
    val rBound: Int,
    lChild: SegmentTree[A],
    rChild: SegmentTree[A],
)(using
    m: Monoid[A]
) extends SegmentTree[A] {
  def this(lChild: SegmentTree[A], rChild: SegmentTree[A])(using m: Monoid[A]) =
    this(
      m.add(lChild.accumulated, rChild.accumulated),
      lChild.lBound,
      rChild.rBound,
      lChild,
      rChild,
    )

  override def accumulated: A = acc

  override def accumulated(l: Int, r: Int): A =
    if !intersects(l, r) then m.zero
    else if l <= lBound && rBound <= r then acc
    else m.add(lChild.accumulated(l, r), rChild.accumulated(l, r))

  override def intersects(i: Int): Boolean = lBound <= i && i <= rBound

  override def intersects(l: Int, r: Int): Boolean =
    (l <= lBound && rBound <= r) || intersects(l) || intersects(r)

  override def update(i: Int, x: A): SegmentTree[A] =
    if intersects(i) then
      lChild.update(i, x)
      rChild.update(i, x)
      acc = m.add(lChild.accumulated, rChild.accumulated)
    this
}

class SegmentTreeLeaf[A](i: Int, private var x: A)(using m: Monoid[A]) extends SegmentTree[A] {
  override def update(j: Int, y: A): SegmentTree[A] =
    if intersects(j) then x = y
    this

  override def accumulated: A = x

  override def accumulated(l: Int, r: Int): A =
    if intersects(l, r) then x
    else m.zero

  override def intersects(j: Int): Boolean         = i == j
  override def intersects(l: Int, r: Int): Boolean = l <= i && i <= r

  override def lBound: Int = i
  override def rBound: Int = i
}

class NumArray(segTree: SegmentTree[Int]) {
  def this(items: IterableOnce[Int]) = this(SegmentTree.from(items))
  def update(index: Int, value: Int): Unit = segTree.update(index, value)
  def sumRange(left: Int, right: Int): Int = segTree.accumulated(left, right)
}
