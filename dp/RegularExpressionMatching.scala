import scala.collection.mutable.ArrayBuffer
import scala.collection.Seq

enum Pattern {
  case Lit(c: Char)
  case Any
  case Rep(p: Pattern)

  infix def matches(c: Char): Boolean =
    this match
      case Any     => true
      case Lit(ch) => c == ch
      case Rep(p)  => p matches c

  def isRep: Boolean = this.isInstanceOf[Rep]
}

def parsePattern(s: String): ArrayBuffer[Pattern] =
  var res = ArrayBuffer[Pattern]()
  for c <- s do
    c match
      case '*' => res(res.length - 1) = Pattern.Rep(res(res.length - 1))
      case '.' => res += Pattern.Any
      case c   => res += Pattern.Lit(c)
  res

object Solution:
  def isMatch(s: String, p: String): Boolean = parsePattern(p) matches s

extension (pat: Seq[Pattern])
  infix def matches(s: String): Boolean =
    val n -> m = pat.length -> s.length
    val table  = Array.fill(n + 1, m + 1)(false)
    table(0)(0) = true
    for
      i <- 1 to n
      curPat = pat(i - 1)
    do table(i)(0) = table(i - 1)(0) && curPat.isRep
    for
      i <- 1 to n
      j <- 1 to m
      curPat  = pat(i - 1)
      curChar = s(j - 1)
    do
      val matchesEmpty      = curPat.isRep && table(i - 1)(j)
      val matchesRepetition = curPat.isRep && (curPat matches curChar) && table(i)(j - 1)
      val matchesSingleChar = (curPat matches curChar) && table(i - 1)(j - 1)
      table(i)(j) = matchesEmpty || matchesRepetition || matchesSingleChar
    table(n)(m)

