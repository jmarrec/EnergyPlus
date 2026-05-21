python scripts/dev/update_space_field.py

commit_msg=$(cat << 'EOF'
[auto] Update testfiles/datasets and C++ tests - Space Default Construction Set Name

```bash
python scripts/dev/update_space_field.py
```
EOF
)

echo "$commit_msg"

git add -u
git commit -F - <<< "$commit_msg"
