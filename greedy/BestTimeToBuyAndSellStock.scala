// https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/submissions/1755434081/

object Solution:
  def maxProfit(prices: Array[Int]): Int =
    var held: Option[Int] = None
    var profit            = 0
    for {
      (cur, i) <- prices.zipWithIndex
      next = prices.lift(i + 1).getOrElse(0)
    } do
      held match
        case None => if cur < next then held = Some(cur)
        case Some(value) =>
          if next < cur then
            profit += cur - value
            held = None
    math.max(0, profit)
