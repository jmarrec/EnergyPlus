reformat_class() {

  python scripts/dev/split_space_load_idf.py          --only-class $1
  python scripts/dev/split_space_load_unit_tests.py   --only-class $1
  python scripts/dev/split_space_load_epjson.py       --only-class $1

  commit_msg=$(cat << EOF
[auto] $1: split into Definition - testfiles/datasets

\`\`\`bash
python scripts/dev/split_space_load_idf.py          --only-class $1
python scripts/dev/split_space_load_unit_tests.py   --only-class $1
python scripts/dev/split_space_load_epjson.py       --only-class $1
\`\`\`
EOF
)

echo "$commit_msg"

git add -u
git commit -F - <<< "$commit_msg"

}

reformat_class "$1"
