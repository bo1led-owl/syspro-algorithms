object PersistentStack {
  def apply[A](): PersistentStack[A] = EmptyPersistentStack
}

trait PersistentStack[+A] {
  def top: Option[A]
  def pop: Option[(PersistentStack[A], A)]
  def push[B >: A](x: B): PersistentStack[B]
  def size: Int
}

case object EmptyPersistentStack extends PersistentStack[Nothing] {
  override def size: Int                         = 0
  override def top: Option[Nothing]              = None
  override def pop: Option[Nothing]              = None
  override def push[B](x: B): PersistentStack[B] = NonEmptyPersistentStack(x, this, 1)
}

case class NonEmptyPersistentStack[A](item: A, prev: PersistentStack[A], size: Int)
    extends PersistentStack[A] {
  override def top: Option[A]                         = Some(item)
  override def pop: Option[(PersistentStack[A], A)]   = Some(prev -> item)
  override def push[B >: A](x: B): PersistentStack[B] = NonEmptyPersistentStack(x, this, size + 1)
}

class PersistentQueue[A] private (
    private val l: PersistentStack[A],
    private val lPrime: PersistentStack[A],
    private val r: PersistentStack[A],
    private val rPrime: PersistentStack[A],
    private val s: PersistentStack[A],
    private val recopy: Boolean,
    private val toCopy: Int,
    private val copied: Boolean,
) {
  def this() = this(
    PersistentStack(),
    PersistentStack(),
    PersistentStack(),
    PersistentStack(),
    PersistentStack(),
    false,
    0,
    false,
  )

  private def additionalOperations: PersistentQueue[A] = {
    var toDo      = 3
    var newR      = r
    var newS      = s
    var curCopied = copied

    while !curCopied && toDo > 0 && newR.size > 0 do {
      val (rn, x) = newR.pop.get
      newR = rn
      newS = newS.push(x)
      toDo -= 1
    }

    var newL = l
    while toDo > 0 && newL.size > 0 do {
      curCopied = true
      val (ln, x) = newL.pop.get
      newL = ln
      newR = newR.push(x)
      toDo -= 1
    }

    var curCopy = toCopy
    while toDo > 0 && newS.size > 0 do {
      val (sn, x) = newS.pop.get
      newS = sn
      if curCopy > 0 then {
        newR = newR.push(x)
        curCopy -= 1
      }
      toDo -= 1
    }

    val newLPrime = lPrime
    if s.size > 0 then
      PersistentQueue(newL, newLPrime, newR, rPrime, newS, recopy, curCopy, curCopied)
    else PersistentQueue(newLPrime, newL, newR, rPrime, newS, recopy, curCopy, curCopied)
  }

  private def checkRecopy: PersistentQueue[A] =
    if l.size > r.size then
      PersistentQueue(l, lPrime, r, rPrime, s, true, r.size, false).checkNormal
    else PersistentQueue(l, lPrime, r, rPrime, s, false, toCopy, copied)

  private def checkNormal: PersistentQueue[A] =
    val newQ = additionalOperations
    PersistentQueue(
      newQ.l,
      newQ.lPrime,
      newQ.r,
      newQ.rPrime,
      newQ.s,
      newQ.s.size > 0,
      0,
      newQ.copied,
    )

  def isEmpty: Boolean = !recopy && r.size == 0

  def push[B >: A](x: B): PersistentQueue[B] =
    if !recopy then
      PersistentQueue(l.push(x), lPrime, r, rPrime, s, recopy, toCopy, copied).checkRecopy
    else PersistentQueue(l, lPrime.push(x), r, rPrime, s, recopy, toCopy, copied).checkNormal

  def pop: Option[(PersistentQueue[A], A)] =
    if !recopy then
      r.pop map { (newR, x) =>
        PersistentQueue(l, lPrime, newR, rPrime, s, recopy, toCopy, copied).checkRecopy -> x
      }
    else
      rPrime.pop map { (newRPrime, x) =>
        val curCopy   = if toCopy > 0 then toCopy - 1 else toCopy
        var (newR, y) = if toCopy == 0 then r.pop.get else r -> x
        PersistentQueue(l, lPrime, newR, newRPrime, s, recopy, curCopy, copied).checkNormal -> y
      }
}

@main def main: Unit = {
  val q = PersistentQueue[Int]()

  assert(q.isEmpty)
  assert(q.pop.isEmpty)

  val q2 = q.push(42)

  assert(!q2.isEmpty)
  assert(q2.pop.get._2 == 42)

  val q3 = q2.push(37)

  assert(!q3.isEmpty)

  val (q4, x) = q3.pop.get

  assert(!q4.isEmpty)
  assert(x == 42)
  assert(q4.pop.get._2 == 37)
}
