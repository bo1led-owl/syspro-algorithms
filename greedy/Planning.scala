import scala.collection.mutable.ArrayBuffer
import scala.collection.Seq

val tests = List(
  List(
    Task("D", 2, 50),
    Task("C", 0, 30),
    Task("A", 2, 25),
    Task("E", 2, 20),
    Task("B", 3, 10),
  ) -> List("C", "A", "D", "B", "E"),
  List(
    Task("D", 2, 50),
    Task("C", 0, 20),
    Task("A", 2, 25),
    Task("E", 3, 20),
    Task("B", 4, 30),
    Task("F", 3, 40),
    Task("G", 5, 35),
    Task("H", 1, 10),
  ) -> List("C", "A", "D", "E", "B", "G", "H", "F"),
)

@main def main: Unit =
  runTests(solve, "union-find")
  runTests(naive, "naive")

def runTests(solution: Seq[Task] => Seq[Task], name: String): Unit = {
  println(s"Testing $name...")
  val failedTests = tests.view map
    { (tasks, answer) => solution(tasks).map(_.name) -> answer } filter
    { (actual, expected) => actual != expected }
  if failedTests.isEmpty
  then println("OK")
  else
    def fmtAnswer(a: IterableOnce[String]): String = a.iterator mkString " -> "
    failedTests foreach { (actual, expected) =>
      println(s"Failed: expected ${fmtAnswer(expected)}, got ${fmtAnswer(actual)}")
    }
}

def solve(tasks: Seq[Task]): Array[Task] = {
  val n   = tasks.length
  val uf  = UnionFind(n)
  val res = Array.fill[Task](n)(null)

  for task @ Task(_, deadline, _) <- tasks do
    res.update(uf.leftmost(deadline), task)
    val left = (uf.leftmost(deadline) - 1) modulo n
    uf.union(deadline, left)

  assert(res forall (_ != null))
  res
}

def naive: Seq[Task] => Seq[Task] = identity

case class Task(name: String, deadline: Int, penalty: Int)

extension (i: Int)
  infix def modulo(j: Int): Int =
    if i < 0 && i % j != 0 then j + i % j else i % j

class UnionFind private (
    parents: Array[Int],
    ranks: Array[Int],
    lefts: Array[Int],
) extends (Int => Int) {
  def this(n: Int) = this(
    Array.from(0 until n),
    Array.fill(n)(0),
    Array.from(0 until n),
  )

  def apply(item: Int): Int =
    val parent = parents(item)
    if parent != item then
      val set = this(parent)
      parents.update(item, set)
      lefts.update(item, lefts(set))
      set
    else item

  def union(a: Int, b: Int): Unit =
    val aClass -> bClass = this(a)       -> this(b)
    val aRank -> bRank   = ranks(aClass) -> ranks(bClass)
    if aRank > bRank then
      lefts.update(aClass, leftmost(b))
      parents.update(bClass, aClass)
    else
      lefts.update(bClass, leftmost(b))
      parents.update(aClass, bClass)
      if aRank == bRank then ranks.update(bClass, bRank + 1)

  def leftmost(i: Int): Int = lefts(this(i))
}
