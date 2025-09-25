import scala.collection.mutable.ArrayBuffer
import scala.collection.mutable.HashSet
import scala.collection.mutable.PriorityQueue

import scala.util.matching.Regex

@main def main(args: String*) = {
  val input  = scala.io.Source.fromFile(args(0)).getLines
  val (n, _) = parseHeader(input.next)

  val graph = Graph(n)(input map parseLine)

  var accessPoints = 0
  var totalWeight  = 0
  val visited      = HashSet[Int]()

  Iterator range (0, n) filterNot visited map (graph.prims) foreach
    { (newVisited, newWeight) =>
      accessPoints += 1
      visited ++= newVisited
      totalWeight += newWeight
    }

  println(s"$accessPoints $totalWeight")
}

class Graph(repr: Array[ArrayBuffer[Edge]]) {
  def this(n: Int)(edges: IterableOnce[(Int, Int, Int)]) =
    this(Array.fill(n)(ArrayBuffer[Edge]()))
    edges.iterator.foreach(addEdge)

  def addEdge(from: Int, to: Int, weight: Int): Unit =
    repr(from) += Edge(to, weight)
    repr(to) += Edge(from, weight)

  def prims(start: Int): (HashSet[Int], Int) = {
    val visited     = HashSet(start)
    var totalWeight = 0

    val heap = PriorityQueue from repr(start).iterator

    def notVisitedEdge(e: Edge) = !visited(e.dest)
    def getNext: Option[Edge]   = heap dequeueUntil notVisitedEdge

    Iterator continually getNext takeWhile (_.isDefined) map (_.get.unapply) foreach
      { (cur, weight) =>
        visited += cur
        totalWeight += weight
        heap ++= repr(cur).iterator filter notVisitedEdge
      }

    visited -> totalWeight
  }
}

case class Edge(dest: Int, weight: Int) extends Ordered[Edge]:
  override infix def compare(that: Edge): Int = that.weight compare weight
  def unapply: (Int, Int)                     = dest -> weight

given Ordering[Edge] with
  infix def compare(x: Edge, y: Edge): Int = x compare y

extension [T](q: PriorityQueue[T])
  infix def dequeueUntil(p: T => Boolean): Option[T] =
    if q.isEmpty then None
    else Some(q.dequeue) filter p orElse (q dequeueUntil p)

def parseHeader(line: String): (Int, Int) =
  line.trim match
    case s"$n $m" => n.toInt -> m.toInt

def parseLine(line: String): (Int, Int, Int) =
  line.trim match
    case s"$from $to $weight" => (from.toInt, to.toInt, weight.toInt)
