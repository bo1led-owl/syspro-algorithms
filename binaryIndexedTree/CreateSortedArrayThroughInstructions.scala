import scala.collection.Seq

trait Semigroup[A] {
  def add(x: A)(y: A): A
}

implicit class SemigroupOps[A](l: A) {
  infix def add(r: A)(using ev: Semigroup[A]): A = ev.add(l)(r)
}

trait Monoid[A] extends Semigroup[A] {
  def zero: A
}

trait Group[A] extends Monoid[A] {
  def sub(x: A)(y: A): A
}

implicit class GroupOps[A](l: A) {
  infix def sub(r: A)(using ev: Group[A]): A = ev.sub(l)(r)
}

given intAdditionGroup: Group[Int] with
  def add(x: Int)(y: Int): Int = x + y
  def sub(x: Int)(y: Int): Int = x - y
  def zero: Int                = 0

class BinaryIndexedTree[A: scala.reflect.ClassTag] private (repr: Array[A])(using ev: Group[A]) {
  import ev.zero

  def this(n: Int)(using ev: Group[A]) = this(Array.fill(n)(ev.zero))

  private def down(x: Int) = x & (x + 1)
  private def up(x: Int)   = x | (x + 1)

  def increment(pos: Int, diff: A) =
    Iterator
      .iterate(pos)(up)
      .takeWhile { _ < repr.size }
      .foreach { i => repr(i) = repr(i) add diff }

  private def prefSum(pos: Int): A =
    Iterator
      .iterate(pos)(down(_) - 1)
      .takeWhile(_ >= 0)
      .map(repr)
      .foldLeft(zero) { _ add _ }

  def sum(l: Int, r: Int): A = prefSum(r) sub prefSum(l - 1)
}

object Solution {
  val MOD = 1_000_000_007

  def createSortedArray(instructions: Array[Int]): Int = {
    val n   = instructions.max + 1
    val bit = BinaryIndexedTree[Int](n)

    instructions.iterator
      .map { x =>
        bit.increment(x, 1)
        bit.sum(0, x - 1) min bit.sum(x + 1, n - 1)
      }
      .foldLeft(0) { (x, y) => (x + y) % MOD }
  }
}
