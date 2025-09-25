import scala.collection.mutable.ArrayBuffer
import scala.collection.Seq

val tests = (
  List(
    Task("D", 2, 50),
    Task("C", 0, 30),
    Task("A", 2, 25),
    Task("E", 2, 20),
    Task("B", 3, 10),
  ) -> List("C", "A", "D", "B", "E")
) :: (
  List(
    Task("D", 2, 50),
    Task("C", 0, 20),
    Task("A", 2, 25),
    Task("E", 3, 20),
    Task("B", 4, 30),
    Task("F", 3, 40),
    Task("G", 5, 35),
    Task("H", 1, 10),
  ) -> List("C", "A", "D", "E", "B", "G", "H", "F")
) :: Nil

@main def main: Unit =
  runTests(solve, "union-find")
  runTests(naive, "naive")

def runTests(solution: Seq[Task] => Seq[Task], name: String): Unit =
  def fmtAnswer(a: IterableOnce[String]): String = a.iterator mkString " -> "
  println(s"Testing $name")
  val testsRes = tests.foldLeft(true) { (res, test) =>
    val actual = solution(test._1).map(_.name)
    if actual != test._2 then
      println(s"Failed: expected ${fmtAnswer(test._2)}, got ${fmtAnswer(actual)}")
    res && actual == test._2
  }
  if testsRes then println("OK")

def solve(tasks: Seq[Task]): ArrayBuffer[Task] =
  val n   = tasks.length
  val uf  = UnionFind(n)
  val res = ArrayBuffer.fill[Task](n)(null)
  for task @ Task(_, deadline, _) <- tasks do
    res.update(uf(deadline), task)
    uf.union(deadline, (uf(deadline) - 1) modulo n)
  assert(res.forall(_ != null))
  res

def naive: Seq[Task] => Seq[Task] = identity

case class Task(name: String, deadline: Int, penalty: Int)

extension (i: Int)
  infix def modulo(j: Int): Int =
    if i < 0 && i % j != 0 then j + i % j else i % j

class UnionFind private (parents: Array[Int], ranks: Array[Int]):
  def this(n: Int) = this(
    Array.from(0 until n),
    Array.fill(n)(0),
  )

  def apply(item: Int): Int =
    val parent = parents(item)
    if parent != item then
      val set = this(parent)
      parents.update(item, set)
      set
    else item

  def union(a: Int, b: Int): Unit =
    val aClass -> bClass = this(a) -> this(b)
    parents.update(aClass, bClass)
