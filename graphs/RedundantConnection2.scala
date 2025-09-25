import scala.collection.mutable.HashMap

import scala.util.boundary
import scala.util.boundary.break

// https://leetcode.com/problems/redundant-connection-ii/submissions/1777965286/

extension [A](o: Option[A]) infix def ifNone(f: => Unit): Option[A] = o.orElse({ f; None })

def findEdgesPointingToTheSameNode(edges: IterableOnce[Edge]): Option[(Edge, Edge)] =
  val prevEdge           = HashMap[Int, Edge]()
  def checkPrev(e: Edge) = prevEdge lift e.to map (_ -> e) ifNone (prevEdge += e.to -> e)
  (edges.iterator map checkPrev find (_.isDefined)).flatten

def solution(edges: Iterable[Edge]): Edge =
  val candidates = findEdgesPointingToTheSameNode(edges)
  val unionFind  = UnionFind(edges.iterator.map(e => e.from max e.to).max)

  def isSecondCandidate(edge: Edge): Boolean = candidates exists (_._2 == edge)
  boundary:
    for
      edge @ Edge(from, to) <- edges.iterator
      if !isSecondCandidate(edge)
    do
      (unionFind lift edge.from) zip (unionFind lift edge.to) match
        case Some(a -> b) if a == b => break(candidates map (_._1) getOrElse edge)
        case _                      => unionFind union (edge.from, edge.to)
    candidates.get._2

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
  def findRedundantDirectedConnection(edges: Array[Array[Int]]): Array[Int] =
    solution(edges.toList.map(e => Edge(e(0), e(1)))).toArray
