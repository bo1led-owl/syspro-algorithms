// https://leetcode.com/problems/unique-binary-search-trees/submissions/1783914027/

object Solution:
  def numTrees(n: Int): Int =
    val mem: Array[Int] = Array.fill(20)(0)
    mem(0) = 1
    mem(1) = 1
    for size <- 2 to n do
      for leftSubtreeSize <- 0 until size do
        mem(size) +=
          mem(leftSubtreeSize) *
            mem(size - leftSubtreeSize - 1)
    mem(n)
