// https://leetcode.com/problems/count-of-smaller-numbers-after-self/submissions/1812971874/

import scala.math.Ordering.Implicits.infixOrderingOps
import scala.collection.Seq
import scala.collection.IndexedSeq
import scala.collection.mutable.ArrayBuffer
import scala.util.boundary
import scala.util.boundary.break

extension (b: Boolean) def toInt: Int = if b then 1 else 0

extension [A](s: IndexedSeq[A])
  def lowerBound(x: A)(using ord: Ordering[A]): Int = {
    var low  = 0
    var high = s.size

    while low < high do
      val mid = low + (high - low) / 2
      if s(mid) < x then low = mid + 1
      else high = mid
    low
  }

extension [A](a: IndexedSeq[A])
  infix def merge(b: IndexedSeq[A])(using ord: Ordering[A]): IndexedSeq[A] = {
    var i = 0
    var j = 0

    var res = ArrayBuffer[A]()
    boundary:
      while true do
        a.lift(i) -> b.lift(j) match
          case (Some(x), Some(y)) =>
            if x <= y then
              res += x
              i += 1
            else
              res += y
              j += 1
          case (Some(x), None) =>
            res += x
            i += 1
          case (None, Some(y)) =>
            res += y
            j += 1
          case (None, None) => break()
    res
  }

object MergeSortTree {
  def from[A](items: IterableOnce[A])(using ord: Ordering[A]): MergeSortTree[A] = {
    var nodes: List[MergeSortTree[A]] = items.iterator.zipWithIndex
      .map((x, i) => MergeSortTreeLeaf(x, i))
      .toList

    while nodes.size > 1 do
      nodes = nodes
        .grouped(2)
        .map {
          case x :: y :: Nil => MergeSortTreeNode[A](x, y)
          case x :: Nil      => x
          case _             => ???
        }
        .toList

    nodes.head
  }
}

sealed trait MergeSortTree[A](using ord: Ordering[A]) extends ((Int, Int, A) => Int) {
  override def apply(l: Int, r: Int, x: A): Int;
  def items: IndexedSeq[A]
  def lBound: Int
  def rBound: Int
  def intersects(l: Int, r: Int): Boolean =
    (lBound <= l && r <= rBound)
      || (l <= lBound && lBound <= r)
      || (l <= rBound && rBound <= r)
}

class MergeSortTreeLeaf[A](value: A, idx: Int)(using ord: Ordering[A]) extends MergeSortTree[A] {
  override def items: IndexedSeq[A]             = ArrayBuffer(value)
  override def lBound: Int                      = idx
  override def rBound: Int                      = idx
  override def apply(l: Int, r: Int, x: A): Int = (intersects(l, r) && value < x).toInt
}

class MergeSortTreeNode[A] private (
    val items: IndexedSeq[A],
    val lBound: Int,
    val rBound: Int,
    lChild: MergeSortTree[A],
    rChild: MergeSortTree[A],
)(using ord: Ordering[A])
    extends MergeSortTree[A] {
  override def apply(l: Int, r: Int, x: A): Int =
    if !intersects(l, r) then 0
    else if l <= lBound && rBound <= r then items.lowerBound(x)
    else lChild(l, r, x) + rChild(l, r, x)

  def this(lc: MergeSortTree[A], rc: MergeSortTree[A])(using ord: Ordering[A]) =
    this(lc.items merge rc.items, lc.lBound, rc.rBound, lc, rc)
}

object Solution:
  def countSmaller(nums: Seq[Int]): List[Int] =
    val mst = MergeSortTree.from(nums)
    nums.zipWithIndex.map { (x, i) => mst(i, nums.size - 1, x) }.toList
