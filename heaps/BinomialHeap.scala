import scala.math.Ordering.Implicits.infixOrderingOps
import scala.collection.mutable.HashMap
import scala.annotation.tailrec

class BinomialHeap[A, K](using ord: Ordering[A], mapper: A => K = identity) {
  private var top: List[Node[A]]         = Nil
  private var nodes: HashMap[K, Node[A]] = HashMap()

  infix def addOne(x: A): BinomialHeap[A, K] =
    val n = Node(x)
    nodes += (mapper(x) -> n)
    addTrees(n :: Nil)
  def +=(x: A): BinomialHeap[A, K] = addOne(x)

  infix def addAll(items: IterableOnce[A]): BinomialHeap[A, K] =
    items.iterator foreach addOne
    this
  def ++=(items: IterableOnce[A]): BinomialHeap[A, K] = addAll(items)

  def dequeue: Option[A] =
    top.minByOption(_.value)(using ord) map { minNode =>
      extractTop(minNode)
      nodes -= mapper(minNode.value)
      minNode.value
    }

  private def extractTop(n: Node[A]): Unit =
    assert(n.parent.isEmpty)
    top = top extractNode n
    addTrees(n.disconnectChildren)

  def decreaseElement(x: A): Unit =
    val key = mapper(x)
    if nodes contains key then
      val n = nodes(key)
      if x >= n.value then return
      n.value = x
      sieveUp(n)
    else addOne(x)

  private def swapNodeValues(a: Node[A], b: Node[A]): Unit =
    val tmp = a.value
    a.value = b.value
    nodes.update(mapper(a.value), a)
    b.value = tmp
    nodes.update(mapper(b.value), b)

  @tailrec private def sieveUp(cur: Node[A]): Unit =
    if cur.parent.isEmpty then return
    val parent = cur.parent.get
    if cur.value >= parent.value then return
    swapNodeValues(cur, parent)
    sieveUp(parent)

  private def addTrees(nodes: List[Node[A]]): BinomialHeap[A, K] =
    assert(top forall (_.parent.isEmpty))
    top = top merge nodes
    this
}

object BinomialHeap {
  infix def from[A, K](items: IterableOnce[A])(using
      ord: Ordering[A],
      mapper: A => K = identity,
  ): BinomialHeap[A, K] =
    BinomialHeap[A, K]() addAll items
}

private class Node[A](
    var value: A,
    var leftmostChild: Option[Node[A]] = None,
    var parent: Option[Node[A]] = None,
    var neighbour: Option[Node[A]] = None,
    var degree: Int = 0,
) {
  infix def merge(that: Node[A])(using ord: Ordering[A]): Node[A] =
    val (x, y) = if this.value <= that.value then (this, that) else (that, this)
    assert(y.parent.isEmpty)
    assert(y.neighbour.isEmpty)
    y.parent = Some(x)
    y.neighbour = x.leftmostChild
    x.leftmostChild = Some(y)
    x.degree += 1
    x

  def disconnectChildren: List[Node[A]] =
    var children: List[Node[A]] = Nil
    var cur                     = leftmostChild
    while cur.isDefined do
      val unwrapped = cur.get
      children +:= unwrapped
      val next = unwrapped.neighbour
      unwrapped.neighbour = None
      unwrapped.parent = None
      cur = next
    assert(children forall (_.parent.isEmpty))
    assert(children forall (_.neighbour.isEmpty))
    children
}

extension (o: Option[Node[?]]) def getDegree: Int = o map { _.degree } getOrElse Int.MaxValue

private def min[A](a: A, b: A, c: A)(using ord: Ordering[A]) = (a min b) min c

extension [A](l: List[Node[A]])
  infix def extractNode(n: Node[A]): List[Node[A]] =
    l.takeWhile(_ != n) ++ l.dropWhile(_ != n).tail

  infix def merge(l2: List[Node[A]])(using ord: Ordering[A]): List[Node[A]] = {
    var res: List[Node[A]] = Nil
    var x                  = l
    var y                  = l2

    var carry: Option[Node[A]] = None
    while !x.isEmpty || !y.isEmpty || carry.isDefined do {
      val minDegree = min(x.headOption.getDegree, y.headOption.getDegree, carry.getDegree)

      (x, y, carry) match
        case (i :: xt, j :: yt, Some(c))
            if i.degree == minDegree
              && j.degree == minDegree
              && c.degree == minDegree =>
          res :+= c
          carry = Some(i merge j)
          x = xt
          y = yt
        case (i :: xt, j :: yt, None)
            if i.degree == minDegree
              && j.degree == minDegree =>
          carry = Some(i merge j)
          x = xt
          y = yt
        case (i :: xt, _, Some(c))
            if i.degree == minDegree
              && c.degree == minDegree =>
          carry = Some(i merge c)
          x = xt
        case (_, j :: yt, Some(c))
            if j.degree == minDegree
              && c.degree == minDegree =>
          carry = Some(j merge c)
          y = yt
        case (i :: xt, _, _) if i.degree == minDegree =>
          res :+= i
          x = xt
        case (_, j :: yt, _) if j.degree == minDegree =>
          res :+= j
          y = yt
        case (_, _, Some(c)) if c.degree == minDegree =>
          res :+= c
          carry = None
        case _ => ()
    }

    assert(res forall (_.neighbour.isEmpty))
    assert(res forall (_.parent.isEmpty))
    res
  }
