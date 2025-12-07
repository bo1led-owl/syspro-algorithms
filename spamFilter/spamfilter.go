package spamfilter

type trieNode struct {
	TerminalFor    *string
	Parent         *trieNode
	EdgeFromParent rune
	SuffixLink     *trieNode
	Edges          map[rune](*trieNode)
	Visits         int
}

type SpamFilter struct {
	trieRoot  *trieNode
	trieNodes [](*trieNode)
}

func (sf *SpamFilter) newNode(parent *trieNode, edgeFromParent rune) *trieNode {
	res := &trieNode{
		Parent:         parent,
		EdgeFromParent: edgeFromParent,
		Edges:          make(map[rune](*trieNode)),
	}
	sf.trieNodes = append(sf.trieNodes, res)
	return res
}

func (sf *SpamFilter) AddTrigger(trigger string) {
	for _, node := range sf.trieNodes {
		node.SuffixLink = nil
	}

	if sf.trieRoot == nil {
		sf.trieRoot = sf.newNode(nil, 0)
	}

	curNode := sf.trieRoot
	for _, c := range trigger {
		nextNode, ok := curNode.Edges[c]

		if ok {
			curNode = nextNode
			continue
		}

		nextNode = sf.newNode(curNode, c)
		curNode.Edges[c] = nextNode

		curNode = nextNode
	}

	curNode.TerminalFor = &trigger
}

type Result struct {
	Indices      map[string]([]int)
	TotalMatches int
}

func (sf *SpamFilter) Check(text string) Result {
	if sf.trieRoot == nil {
		return Result{}
	}

	indices := make(map[string]([]int))
	total := 0

	curNode := sf.trieRoot
	for i, c := range text {
		curNode.Visits += 1

		suffixBacktrackNode := curNode
		for suffixBacktrackNode != sf.trieRoot {
			trigger := suffixBacktrackNode.TerminalFor
			if trigger != nil {
				indices[*trigger] = append(indices[*trigger], i-len(*trigger))
				total += 1
			}

			suffixBacktrackNode = sf.suf(suffixBacktrackNode)
		}

		curNode = sf.next(curNode, c)
	}

	return Result{indices, total}
}

func (sf *SpamFilter) suf(v *trieNode) *trieNode {
	if v.SuffixLink == nil {
		if v == sf.trieRoot || v.Parent == sf.trieRoot {
			v.SuffixLink = sf.trieRoot
		} else {
			v.SuffixLink = sf.next(sf.suf(v.Parent), v.EdgeFromParent)
		}
	}
	return v.SuffixLink
}

func (sf *SpamFilter) next(v *trieNode, c rune) *trieNode {
	res, present := v.Edges[c]
	if present {
		return res
	}

	if v == sf.trieRoot {
		return sf.trieRoot
	} else {
		return sf.next(sf.suf(v), c)
	}
}
