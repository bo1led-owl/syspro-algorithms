## Running

Unzip `testrepo.zip`, then execute
```
go run main.go testrepo/ 2b24b0f dcb6409 "python -m unittest calc.py"
```
from this (`bisect`) directory.
Output should be `3c1a963`. Commits passed as boundaries in the example are the second and one-before-the-last respectively.
