#!/usr/bin/env bash
# Wait for one job of a CI run to finish, then print its status, failed
# steps and error lines.  Usage: tools/ci_job.sh <run-id> <job-name-regex>
GH="${GH:-/c/Program Files/GitHub CLI/gh.exe}"
run=$1; pat=$2
until s=$("$GH" run view "$run" --json jobs --jq ".jobs[] | select(.name|test(\"$pat\")) | .status + \" \" + (.conclusion // \"\")"); [[ "$s" == completed* ]]; do sleep 20; done
echo "$pat: $s"
id=$("$GH" run view "$run" --json jobs --jq ".jobs[] | select(.name|test(\"$pat\")) | .databaseId")
"$GH" run view "$run" --json jobs --jq ".jobs[] | select(.name|test(\"$pat\")) | [.steps[] | select(.conclusion==\"failure\") | .name] | join(\", \")"
"$GH" api --allow-escape-sequences "repos/Leishem/MGLTools-Py3/actions/jobs/$id/logs" > "build/ci-$pat.log" 2>&1
grep -nE "error:|fatal error|Error|FAILED|failures in|passed|failed" "build/ci-$pat.log" | sed 's/^[0-9]*:[^ ]* //' | cut -c1-230 | head -${N:-25}
