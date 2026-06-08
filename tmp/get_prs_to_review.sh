gh pr list \
  --repo NREL/EnergyPlus \
  --state open \
  --limit 100 \
  --json number,title,author,createdAt,url,isDraft \
  --jq '
    .[]
    | .age_days = (((now - (.createdAt | fromdateiso8601)) / 86400) | floor)
    | select(.isDraft == false and .age_days < 365)
    | {
        number,
        age_days,
        author: .author.login,
        title,
        url
      }
  '
