import scala.annotation.tailrec

// https://leetcode.com/problems/jump-game/submissions/1755389743/

object Solution:
  @tailrec def canJump(nums: Array[Int], i: Int = 0, maxReachable: Int = 0): Boolean =
    if maxReachable >= nums.length - 1 then true
    else if i > maxReachable then false
    else canJump(nums, i + 1, math.max(maxReachable, i + nums(i)))
