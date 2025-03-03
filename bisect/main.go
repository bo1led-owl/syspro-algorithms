package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
	"slices"
	"strings"
)

func printErrAndExit(code int, format string, a ...any) {
	fmt.Fprintf(os.Stderr, format+"\n", a...)
	os.Exit(code)
}

func getHashes(from string, to string) ([]string, error) {
	output, err := exec.Command("git", "log", "--pretty=%h").Output()
	if err != nil {
		return nil, err
	}

	hashes := strings.Split(string(output), "\n")
	slices.Reverse(hashes)

	for len(hashes) > 0 && hashes[0] != from {
		hashes = hashes[1:]
	}

	if len(hashes) == 0 {
		return nil, errors.New("Couldn't find range starting hash")
	}

	end := -1
	for i := range hashes {
		if hashes[i] == to {
			end = i + 1
			break
		}
	}

	if end == -1 {
		return nil, errors.New("Couldn't find range terminating hash")
	}

	hashes = hashes[:end]
	return hashes, nil
}

func checkout(dest string) error {
	_, err := exec.Command("git", "checkout", dest).Output()
	return err
}

func branch() (string, error) {
	out, err := exec.Command("git", "branch", "--show-current").Output()
	if err != nil {
		return "", err
	}

	return strings.Trim(string(out), "\n \t"), nil
}

func main() {
	if len(os.Args)-1 < 4 {
		printErrAndExit(2, "Too few arguments passed. Expected 4, got %d", len(os.Args)-1)
	}
	if len(os.Args)-1 > 4 {
		printErrAndExit(2, "Too many arguments passed. Expected 4, got %d", len(os.Args)-1)
	}

	repo := os.Args[1]
	startHash := os.Args[2]
	endHash := os.Args[3]
	checkCmd := os.Args[4]
	err := os.Chdir(repo)
	if err != nil {
		printErrAndExit(1, "Failed to `cd` into `%s`: %s", repo, err.Error())
	}

	curBranch, err := branch()
	if err != nil {
		printErrAndExit(1, "Failed to get current branch name: %s", err.Error())
	}

	hashes, err := getHashes(startHash, endHash)
	if err != nil {
		printErrAndExit(1, "Failed to get commit hashes: %s", err.Error())
	}

	l := 0
	r := len(hashes) - 1
	for l < r {
		m := (l + r) / 2

		curHash := hashes[m]

		err = checkout(curHash)
		if err != nil {
			printErrAndExit(1, "Failed to checkout: %s", err.Error())
		}

		err := exec.Command("sh", "-c", checkCmd).Run()
		if err != nil {
			r = m
		} else {
			l = m + 1
		}
	}

	fmt.Println(hashes[l])
	err = checkout(curBranch)
	if err != nil {
		printErrAndExit(1, "Failed to checkout: %s", err.Error())
	}
}
