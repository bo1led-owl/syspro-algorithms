## Running

Unzip `testrepo.zip` (git does not allow to easily commit subrepositories, it has to be a submodule, so it has to be pushed to GitHub by itself, that's not ok)

Execute
```
go run main.go testrepo/ 2b24b0f dcb6409 "python -m unittest calc.py"
```
from this directory.
Output should be `3c1a963`. Commits passed as boundaries in the example are the second and one-before-the-last respectively.
