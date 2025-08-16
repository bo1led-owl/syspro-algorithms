import scala.io.Source
import scala.collection.mutable.HashMap
import scala.collection.mutable.HashSet

type Graph[T] = HashMap[T, List[T]]

def parseFunctions(lines: IterableOnce[String]): Graph[String] =
  def parseFunction(line: String) =
    val pattern = "\\s*(\\w+)\\s*:\\s*(.*)".r
    line match
      case pattern(name, callees) => (name, ",\\s*".r.split(callees).toList)
  HashMap.from(lines.iterator.map(parseFunction))

@main def main() =
  val fns = parseFunctions(Source.stdin.getLines())
  println("SCCs:")
  val sccs = Kosaraju(fns).run
  for scc <- sccs do println(s" - ${scc.mkString(", ")}")

  val recursiveFunctions = {
    val (singleNodeSccs, multipleNodeSccs) = sccs.partition(_.length == 1)
    singleNodeSccs.flatten.filter(fn => fns(fn).contains(fn))
      ++ multipleNodeSccs.flatten
  }.distinct.sorted

  println(s"recursive functions: ${recursiveFunctions.mkString(", ")}")

class Kosaraju[T](graph: Graph[T], revgraph: Graph[T]):
  private var time: HashMap[T, Int] = HashMap()
  private var maxTime: Int          = 0
  def this(g: Graph[T]) = this(g, reverseGraph(g))
  def run: List[List[T]] =
    var timeDfs = DFS[T](
      revgraph,
      onExit = node => {
        time += (node, maxTime); maxTime += 1
      },
    )
    revgraph.keys.foreach(timeDfs.run)
    var sccs: List[List[T]] = Nil
    var curScc: List[T]     = Nil
    var sccDfs              = DFS(graph, onEnter = node => curScc :+= node)
    for v <- graph.keys.toList.filter(time.contains).sortBy(time).reverse do
      curScc = Nil
      sccDfs.run(v)
      if !curScc.isEmpty then sccs :+= curScc
    sccs

class DFS[T](
    graph: Graph[T],
    onEnter: T => Unit = (_: T) => (),
    onExit: T => Unit = (_: T) => (),
):
  private var visited: HashSet[T] = HashSet()
  def run(v: T): Unit =
    if !graph.contains(v) then return
    if visited.contains(v) then return
    visited += v
    onEnter(v)
    graph(v).foreach(run)
    onExit(v)

def reverseGraph[T](g: Graph[T]): Graph[T] =
  var newGraph: Graph[T] = HashMap()
  for
    (v, edges) <- g
    u          <- edges
  do
    if !newGraph.contains(u) then newGraph += (u, Nil)
    newGraph(u) :+= v
  newGraph
