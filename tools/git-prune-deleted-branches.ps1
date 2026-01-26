git fetch -p
git branch -vv | Where-Object { $_ -match ': gone]' } | ForEach-Object { $_.Trim().Split(' ')[0] } | ForEach-Object { git branch -D $_ }
pause
