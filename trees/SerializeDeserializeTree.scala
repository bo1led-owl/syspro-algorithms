// https://leetcode.com/problems/serialize-and-deserialize-binary-tree/submissions/1589109500/

class TreeNode(var value: Int, var left: TreeNode = null, var right: TreeNode = null)

class Codec {
  def serialize(root: TreeNode): String =
    if root == null
    then "ы"
    else s"а${root.value}${serialize(root.left)}${serialize(root.right)}я"

  private def deserializeInternal(data: String): (TreeNode, String) =
    if data(0) == 'ы'
    then (null, data.slice(1, data.length))
    else
      val value   = data.slice(1, data.length).takeWhile(c => c == '-' || c.isDigit)
      println(value)
      val (l, s1) = deserializeInternal(data.slice(1 + value.length, data.length))
      val (r, s2) = deserializeInternal(s1)
      (TreeNode(value.toInt, l, r), s2.slice(1, s2.length))

  def deserialize(data: String): TreeNode = deserializeInternal(data)._1
}
