import scala.collection.mutable.HashMap

import scala.util.boundary
import scala.util.boundary.break

// https://leetcode.com/problems/redundant-connection/submissions/1777964439/

def solution(edges: List[Edge]): Edge =
  val unionFind = UnionFind(edges.map(e => e.from max e.to).max)
  boundary:
    for edge @ Edge(from, to) <- edges do
      (unionFind.lift(from) -> unionFind.lift(to)) match
        case (Some(a), Some(b)) if a == b => break(edge)
        case _                            => unionFind union (from, to)
    ???

case class Edge(from: Int, to: Int):
  def toArray: Array[Int] = Array(from, to)

class UnionFind private (val parents: Array[Int], val ranks: Array[Int])
    extends PartialFunction[Int, Int] {
  def this(n: Int) = this(Array.fill(n + 1)(-1), Array.fill(n + 1)(0))

  def apply(item: Int): Int =
    val parent = parents(item)
    if parent != item then
      val set = this(parent)
      parents.update(item, set)
      set
    else item

  override def isDefinedAt(x: Int): Boolean = parents(x) != -1

  def insert(item: Int): Unit = {
    if isDefinedAt(item) then return

    parents.update(item, item)
    ranks.update(item, 0)
  }

  def findOrInsert(item: Int): Int =
    this lift item getOrElse {
      insert(item)
      item
    }

  infix def union(a: Int, b: Int): Unit = {
    val aClass = findOrInsert(a)
    val bClass = findOrInsert(b)

    val aRank = ranks(aClass)
    val bRank = ranks(bClass)
    if aRank > bRank then parents.update(bClass, aClass)
    else
      parents.update(aClass, bClass)
      if aRank == bRank then ranks.update(bClass, bRank + 1)
  }
}

object Solution:
  def findRedundantConnection(edges: Array[Array[Int]]): Array[Int] =
    solution(
      edges.toList.map(e => Edge(e(0), e(1)))
    ).toArray
