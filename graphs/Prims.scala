import scala.collection.mutable.ArrayBuffer
import scala.collection.mutable.HashSet

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
    val visited                 = HashSet(start)
    def notVisitedEdge(e: Edge) = !visited(e.dest)
    var totalWeight             = 0

    val heap = BinomialHeap.from(repr(start))(using mapper = _.dest)

    Iterator continually (heap.dequeue) takeWhile (_.isDefined) map (_.get) foreach
      { case Edge(cur, weight) =>
        visited += cur
        totalWeight += weight
        repr(cur) filter notVisitedEdge foreach heap.decreaseElement
      }

    visited -> totalWeight
  }
}

case class Edge(dest: Int, weight: Int) extends Ordered[Edge]:
  override infix def compare(that: Edge): Int = weight compare that.weight
  def unapply: (Int, Int)                     = dest -> weight

given Ordering[Edge] with
  infix def compare(x: Edge, y: Edge): Int = x compare y

def parseHeader(line: String): (Int, Int) =
  line.trim match
    case s"$n $m" => n.toInt -> m.toInt

def parseLine(line: String): (Int, Int, Int) =
  line.trim match
    case s"$from $to $weight" => (from.toInt, to.toInt, weight.toInt)
