import scala.util.Random
import java.awt.Image

trait Semigroup[A]:
  def add(a: A, b: A): A

trait Monoid[A] extends Semigroup[A]:
  def zero: A

object Treap {
  def apply[A](x: A)(using rand: Random, m: Monoid[A]): Treap[A] = Node(x)
  def empty[A](using rand: Random, m: Monoid[A]): Treap[A]       = Empty()
  def from[A](items: IterableOnce[A])(using rand: Random, m: Monoid[A]): Treap[A] =
    items.iterator.foldLeft(empty[A]) { _ + _ }
}

sealed trait Treap[A](using rand: Random, m: Monoid[A]) {
  def split(at: Int): (Treap[A], Treap[A])

  infix def merge(that: Treap[A]): Treap[A]
  final def ++(that: Treap[A]): Treap[A] = merge(that)

  def size: Int

  def addOne(item: A): Treap[A]   = merge(Treap(item))
  final def +(item: A): Treap[A]  = addOne(item)
  final def :+(item: A): Treap[A] = this ++ Node(item)
  final def +:(item: A): Treap[A] = Node(item) ++ this

  def insert(at: Int, item: A): Treap[A] =
    val (l, r) = split(at - 1)
    l ++ Treap(item) ++ r

  def remove(at: Int): Treap[A] = remove(at, 1)
  def remove(at: Int, count: Int): Treap[A] =
    val (l, r)  = split(at)
    val (_, rr) = r.split(count)
    l ++ rr

  def accumulated: A
  def accumulated(from: Int, to: Int): A =
    val (l, r)   = split(from)
    val (rl, rr) = r.split(to - from)
    rl.accumulated
}

case class Empty[A]()(using rand: Random, m: Monoid[A]) extends Treap[A] {
  override def split(at: Int): (Treap[A], Treap[A]) = (Empty(), Empty())
  override def merge(that: Treap[A]): Treap[A]      = that

  override def size: Int = 0

  override def addOne(item: A): Treap[A]          = Treap(item)
  override def insert(at: Int, item: A): Treap[A] = Treap(item)

  override def remove(at: Int): Treap[A]             = this
  override def remove(at: Int, count: Int): Treap[A] = this

  override def accumulated: A                     = m.zero
  override def accumulated(from: Int, to: Int): A = m.zero
}

case class Node[A](
    value: A,
    priority: Int,
    size: Int,
    accumulated: A,
    left: Treap[A],
    right: Treap[A],
)(using
    rand: Random,
    m: Monoid[A],
) extends Treap[A] {
  def replaceLeft(n: Treap[A]): Node[A]  = Node(value, priority, n, right)
  def replaceRight(n: Treap[A]): Node[A] = Node(value, priority, left, n)

  override def split(at: Int): (Treap[A], Treap[A]) =
    if at <= left.size then
      val (ll, lr) = left.split(at)
      ll -> replaceLeft(lr)
    else
      val (rl, rr) = right.split(at - left.size - 1)
      replaceRight(rl) -> rr

  override def merge(that: Treap[A]): Treap[A] =
    that match
      case Empty() => this
      case that: Node[A] =>
        if priority < that.priority then replaceRight(right ++ that)
        else that.replaceLeft(this ++ that.left)
}

object Node {
  def apply[A](x: A)(using rand: Random, m: Monoid[A]): Node[A] =
    Node(x, rand.nextInt(), 1, x, Empty(), Empty())

  def apply[A](x: A, l: Treap[A], r: Treap[A])(using rand: Random, m: Monoid[A]): Node[A] =
    Node(x, rand.nextInt(), l, r)

  private def apply[A](
      x: A,
      p: Int,
      l: Treap[A],
      r: Treap[A],
  )(using
      rand: Random,
      m: Monoid[A],
  ): Node[A] =
    val c   = 1 + l.size + r.size
    val acc = m.add(x, m.add(l.accumulated, r.accumulated))
    Node(x, p, c, acc, l, r)
}
