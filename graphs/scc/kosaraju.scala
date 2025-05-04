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
  for scc <- Kosaraju(fns).run do println(s"  ${scc.mkString(", ")}")

  var recursiveFunctions = HashSet[String]()
  for fn <- fns.keys do DFS(fns, _ => (), _ => (), recursiveFunctions += _).run(fn)
  println(s"recursive functions: ${recursiveFunctions.mkString(", ")}")

class Kosaraju[T](graph: Graph[T], revgraph: Graph[T]):
  private var time: HashMap[T, Int] = HashMap()
  private var maxTime: Int          = 0
  def this(g: Graph[T]) = this(g, reverseGraph(g))
  def run: List[List[T]] =
    var timeDfs = DFS[T](revgraph, _ => (), node => { time += (node, maxTime); maxTime += 1 })
    for v <- revgraph.keys do timeDfs.run(v)
    var sccs: List[List[T]] = Nil
    var curScc: List[T]     = Nil
    var sccDfs              = DFS(graph, node => curScc :+= node)
    for v <- graph.keys.toList.sortBy(time(_)).reverse do
      curScc = Nil
      sccDfs.run(v)
      if !curScc.isEmpty then sccs :+= curScc
    sccs

class DFS[T](
    graph: Graph[T],
    onEnter: T => Unit = (_: T) => (),
    onExit: T => Unit = (_: T) => (),
    onLoop: T => Unit = (_: T) => ()
):
  private var visited: HashSet[T] = HashSet()
  def run(v: T): Unit =
    if !graph.contains(v) then return
    if visited.contains(v) then
      onLoop(v)
      return
    visited += v
    onEnter(v)
    for u <- graph(v) do run(u)
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
