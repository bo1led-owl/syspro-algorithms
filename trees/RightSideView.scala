import scala.collection.mutable.{ArrayBuffer, ListBuffer}

// https://leetcode.com/problems/binary-tree-right-side-view/submissions/1589118478/

class TreeNode(var value: Int = 0, var left: TreeNode = null, var right: TreeNode = null)

object Solution {
  var layers: ArrayBuffer[ListBuffer[Int]] = null

  private def populateLayers(tree: TreeNode, layer: Int): Unit = {
    if tree == null then return

    if tree.left != null then populateLayers(tree.left, layer + 1)
    if layer >= layers.length then for (_ <- layers.length to layer) layers.append(ListBuffer())
    layers(layer).append(tree.value)
    if tree.right != null then populateLayers(tree.right, layer + 1)
  }

  def rightSideView(root: TreeNode): List[Int] = {
    layers = ArrayBuffer[ListBuffer[Int]]()
    populateLayers(root, 0)
    layers.map(_.last).toList
  }
}
