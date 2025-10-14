import scala.util.Random
import scala.collection.mutable.ArrayBuffer

given intAdditionMonoid: Monoid[Int] with
  def add(a: Int, b: Int): Int = a + b
  def zero: Int                = 0

class TreapSuite extends munit.FunSuite {
  test("merge") {
    val r  = Random()
    val t1 = Treap(42)(using rand = r)
    val t2 = Treap(37)(using rand = r)

    val t = t1 ++ t2

    assertEquals(42 + 37, t.accumulated)
  }

  test("split") {
    val r = Random()

    val a = Treap(42)(using rand = r)
    val b = Treap(37)(using rand = r)
    val c = Treap(14)(using rand = r)
    val d = Treap(7)(using rand = r)

    val t = a ++ b ++ c ++ d
    assertEquals(t.split(0)._2, t)
    assertEquals(t.split(4)._1, t)

    val (t1, t2) = t.split(1)
    assertEquals(42, t1.accumulated)
    assertEquals(37 + 14 + 7, t2.accumulated)

    val (t3, t4) = t2.split(2)
    assertEquals(37 + 14, t3.accumulated)
    assertEquals(7, t4.accumulated)
  }

  test("from") {
    val items = ArrayBuffer(42, 3, 4, 37, 15, 80, 17)

    val t = Treap.from[Int](items)(using rand = Random())

    assertEquals(t.accumulated, items.sum)

    val slices = (0, 4) :: (2, 4) :: (3, 6) :: (1, 2) :: Nil

    slices foreach { (from, to) =>
      assertEquals(
        t.accumulated(from, to),
        items.slice(from, to).sum,
        s"($from, $to)",
      )
    }
  }

  test("insert") {
    var t = Treap(1, 4, 5)(using rand = Random())

    t = t.insert(1, 2);
    assertEquals(t.accumulated, 12)
    t = t.insert(2, 3);
    assertEquals(t.accumulated, 15)
  }

  test("remove") {
    val items = ArrayBuffer(1, 2, 3, 4, 5)

    var t = Treap.from[Int](items)(using rand = Random())

    t = t.remove(2);
    items.remove(2);
    assertEquals(t.accumulated, items.sum)

    t = t.remove(1, 2)
    items.remove(1, 2)
    assertEquals(t.accumulated, items.sum)
  }
}
