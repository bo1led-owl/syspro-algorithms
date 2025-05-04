import scala.io.Source
import scala.collection.mutable.HashMap
import scala.collection.mutable.HashSet

type Graph = HashMap[String, List[String]]

object Graph:
  def fromLines(lines: IterableOnce[String]): Graph =
    def parseFunction(line: String) =
      val pattern = "\\s*(\\w+)\\s*:\\s*(.*)".r
      line match
        case pattern(name, callees) => (name, ",\\s*".r.split(callees).toList)
    HashMap.from(lines.iterator.map(parseFunction))

@main def main() =
  val fns = Graph.fromLines(Source.stdin.getLines())
  println("SCCs:")
  for scc <- Kosaraju(fns).run do println(s"  ${scc.mkString(", ")}")

  var recursiveFunctions = HashSet[String]()
  for fn <- fns.keys do DFS(fns, _ => (), _ => (), recursiveFunctions += _).run(fn)
  println(s"recursive functions: ${recursiveFunctions.mkString(", ")}")

class Kosaraju(graph: Graph, revgraph: Graph):
  private var time: HashMap[String, Int] = HashMap()
  private var maxTime: Int               = 0
  def this(g: Graph) = this(g, reverseGraph(g))
  def run: List[List[String]] =
    var timeDfs = DFS(revgraph, _ => (), node => { time += (node, maxTime); maxTime += 1 })
    for v <- revgraph.keys do timeDfs.run(v)
    var sccs: List[List[String]] = Nil
    var curScc: List[String]     = Nil
    var sccDfs                   = DFS(graph, node => curScc :+= node)
    for v <- graph.keys.toList.sortBy(time(_)).reverse do
      curScc = Nil
      sccDfs.run(v)
      if !curScc.isEmpty then sccs :+= curScc
    sccs

class DFS(
    graph: Graph,
    onEnter: String => Unit = _ => (),
    onExit: String => Unit = _ => (),
    onLoop: String => Unit = _ => ()
):
  private var visited: HashSet[String]                 = HashSet()
  private def impl(v: String, f: String => Unit): Unit = ???

  def run(v: String): Unit =
    if !graph.contains(v) then return
    if visited.contains(v) then
      onLoop(v)
      return
    visited += v
    onEnter(v)
    for u <- graph(v) do run(u)
    onExit(v)

def reverseGraph(g: Graph): Graph =
  var newGraph: Graph = HashMap()
  for
    (v, edges) <- g
    u          <- edges
  do
    if !newGraph.contains(u) then newGraph += (u, Nil)
    newGraph(u) :+= v
  newGraph
