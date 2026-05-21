#!/usr/bin/env bash
# Replay all commits from develop..HEAD onto the current branch,
# re-running generator scripts for any [auto] commit instead of cherry-picking.
set -euo pipefail

BASE=$(git merge-base HEAD develop)

# Collect commits oldest-first before we move HEAD
mapfile -t HASHES   < <(git log --reverse --format="%H" "$BASE"..HEAD)
mapfile -t SUBJECTS < <(git log --reverse --format="%s" "$BASE"..HEAD)

echo "Replaying ${#HASHES[@]} commits on top of develop..."
git reset --hard develop

for i in "${!HASHES[@]}"; do
    hash="${HASHES[$i]}"
    subject="${SUBJECTS[$i]}"

    if [[ "$subject" =~ ^(\[auto\]\ )?Update\ testfiles/datasets\ and\ C\+\+\ tests\ -\ Space\ Default\ Construction\ Set\ Name$ ]]; then
        echo "==> Redoing: ${subject}"
        bash scripts/dev/update_space_field.sh
    else
        echo "==> Cherry-picking: ${subject}"
        git cherry-pick "$hash"
    fi
done

echo "Done."
