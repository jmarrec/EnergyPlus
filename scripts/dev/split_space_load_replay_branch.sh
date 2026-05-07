#!/usr/bin/env bash
# Replay all commits from develop..HEAD onto the current branch,
# re-running split_space_load.sh for any commit whose subject matches
# "<ClassName>: split into Definition - testfiles/datasets".
set -euo pipefail

BASE=$(git merge-base HEAD develop)

# Collect commits oldest-first before we move HEAD
mapfile -t HASHES   < <(git log --reverse --format="%H" "$BASE"..HEAD)
mapfile -t SUBJECTS < <(git log --reverse --format="%s" "$BASE"..HEAD)

# If you had a conflict and need to continue the apply
#BASE_HASH="f53f6b2ce914569dc932cbef3a02c56ade9e6b22"  # People IDD (already applied)
#mapfile -t HASHES   < <(git log --reverse --format="%H" "${BASE_HASH}..SpaceLoadScripts_Rebased")
#mapfile -t SUBJECTS < <(git log --reverse --format="%s" "${BASE_HASH}..SpaceLoadScripts_Rebased")

echo "Replaying ${#HASHES[@]} commits on top of develop..."
git reset --hard develop

for i in "${!HASHES[@]}"; do
    hash="${HASHES[$i]}"
    subject="${SUBJECTS[$i]}"

    if [[ "$subject" =~ ^(\[auto\]\ )?([^:]+):\ split\ into\ Definition\ -\ testfiles/datasets$ ]]; then
        class="${BASH_REMATCH[2]}"
        echo "==> Redoing [${class}]: ${subject}"
        bash scripts/dev/split_space_load.sh "$class"
    else
        echo "==> Cherry-picking: ${subject}"
        git cherry-pick "$hash"
    fi
done

echo "Done."
