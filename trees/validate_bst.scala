// https://leetcode.com/problems/validate-binary-search-tree/submissions/1583955357/

class TreeNode(val value: Int = 0, val left: TreeNode = null, val right: TreeNode = null)

import scala.math.{min, max}
import scala.annotation.elidable.{MINIMUM, MAXIMUM}

object Solution {
    private def isValidBST(root: TreeNode, l: Option[Int], r: Option[Int]): Boolean = {
        if (root == null) return true

        if (l.isDefined && root.value <= l.get) return false
        if (r.isDefined && root.value >= r.get) return false
        
        val lbound = l.map(_ max root.value).getOrElse(root.value)
        val rbound = r.map(_ min root.value).getOrElse(root.value)
        isValidBST(root.left, l, Some(rbound)) && isValidBST(root.right, Some(lbound), r)
    }

    def isValidBST(root: TreeNode): Boolean = isValidBST(root, None, None)
}
