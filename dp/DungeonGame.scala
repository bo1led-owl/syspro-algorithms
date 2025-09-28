// https://leetcode.com/problems/dungeon-game/submissions/1784917452/

object Solution:
  def calculateMinimumHP(dungeon: Array[Array[Int]]): Int =
    val n     = dungeon.length
    val m     = dungeon(0).length
    val hpReq = Array.fill(n + 1, m + 1)(Int.MaxValue)
    hpReq(n)(m - 1) = 1
    hpReq(n - 1)(m) = 1
    for
      i <- (0 to n - 1).reverse
      j <- (0 to m - 1).reverse
    do hpReq(i)(j) = (hpReq(i + 1)(j) min hpReq(i)(j + 1)) - dungeon(i)(j) max 1
    hpReq(0)(0)
