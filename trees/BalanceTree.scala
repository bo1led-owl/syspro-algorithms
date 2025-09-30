import scala.collection.mutable.ArrayBuffer
import scala.collection.IndexedSeqView

// https://leetcode.com/problems/balance-a-binary-search-tree/submissions/1589130208/

class TreeNode(var value: Int = 0, var left: TreeNode = null, var right: TreeNode = null)

def toArrayBuffer(t: TreeNode): ArrayBuffer[Int] =
  if t == null then return ArrayBuffer()
  val l = toArrayBuffer(t.left)
  val r = toArrayBuffer(t.right)
  l :+ t.value :++ r

object Solution {
  private def seqToNode(seq: IndexedSeqView[Int]): TreeNode =
    if seq.isEmpty
    then null
    else
      TreeNode(
        seq(seq.length / 2),
        seqToNode(seq.slice(0, seq.length / 2)),
        seqToNode(seq.slice(seq.length / 2 + 1, seq.length))
      )

  def balanceBST(root: TreeNode): TreeNode = seqToNode(toArrayBuffer(root).view)
}
