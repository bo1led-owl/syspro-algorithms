package spamfilter_test

import (
	"slices"
	"spamfilter"
	"testing"
)

func findOccurences(haystack string, needle string) []int {
	var res []int = nil

	for i := range haystack {
		if haystack[i:min(i+len(needle), len(haystack))] == needle {
			res = append(res, i)
		}
	}

	return res
}

func checkResult(t *testing.T, iteration int, text string, trigger string, res spamfilter.Result) int {
	occ := findOccurences(text, trigger)

	if !slices.Equal(res.Indices[trigger], occ) {
		t.Errorf(
			"%d `%s`: occurences mismatch: expected %v, got %v",
			iteration,
			trigger,
			occ,
			res.Indices[trigger],
		)
	}

	if res.Matches[trigger] != len(occ) {
		t.Errorf(
			"%d `%s`: occurence count mismatch: expected %v, got %v",
			iteration,
			trigger,
			len(occ),
			res.Matches[trigger],
		)
	}

	return len(occ)
}

func TestComplicated(t *testing.T) {
	sf := spamfilter.SpamFilter{}

	text := "abbabbabaabbbabaabbbaab"

	sf.AddTrigger("abb")
	res := sf.Check(text)

	total := 0
	total += checkResult(t, 0, text, "abb", res)

	if total != res.TotalMatches {
		t.Errorf("total matches mismatch: expected %v, got %v", total, res.TotalMatches)
	}

	sf.AddTrigger("abba")
	res = sf.Check(text)

	total = 0
	total += checkResult(t, 1, text, "abb", res)
	total += checkResult(t, 1, text, "abba", res)

	if total != res.TotalMatches {
		t.Errorf("total matches mismatch: expected %v, got %v", total, res.TotalMatches)
	}

	sf.AddTrigger("babba")
	res = sf.Check(text)

	total = 0
	total += checkResult(t, 2, text, "abb", res)
	total += checkResult(t, 2, text, "abba", res)
	total += checkResult(t, 2, text, "babba", res)

	if total != res.TotalMatches {
		t.Errorf("total matches mismatch: expected %v, got %v", total, res.TotalMatches)
	}
}
