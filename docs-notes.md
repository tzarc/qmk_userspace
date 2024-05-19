Python regex for matching markdown links:

```regexp
(?P<mdlink>\[(?P<txt>[^)]+)\]\((?P<url>[^)]+)\))
```

Python regex for matching markdown headers with `:id=...`:

```regexp
(?P<mdheader>(?P<depth>#+)\s+(?P<title>.+)\s+(?P<linkmarker>\:id=(?P<linkid>.*)))
```
