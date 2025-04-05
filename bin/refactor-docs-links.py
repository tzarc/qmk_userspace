#!/usr/bin/env python3

import os, subprocess, shlex, sys, re
from pathlib import Path


def _run(command, capture_output=True, combined_output=False, text=True, **kwargs):
    if isinstance(command, str):
        command = shlex.split(command)
    if capture_output:
        kwargs["stdout"] = subprocess.PIPE
        kwargs["stderr"] = subprocess.PIPE
    if combined_output:
        kwargs["stderr"] = subprocess.STDOUT
    if "stdin" in kwargs and kwargs["stdin"] is None:
        del kwargs["stdin"]
    if text:
        kwargs["universal_newlines"] = True
    return subprocess.run(command, **kwargs)


# -----------------------------------------------------------------------------------------------------------------------

qmk_firmware_dir = Path(os.path.realpath(__file__)).parent.parent / "qmk_firmware"
docs_dir = qmk_firmware_dir / "docs"

# -----------------------------------------------------------------------------------------------------------------------

IMAGE_ELEMENT_PREFIX_PATTERN = re.compile(r'(?P<imgprefix><img(?P<preamble>.*)src="(?P<url>[^"]+)")')
LINK_PATTERN = re.compile(r"(?P<mdlink>\[(?P<txt>[^)]+)\]\((?P<url>[^)]+)\))")
SECTION_WITH_ID_PATTERN = re.compile(r"(?P<mdheader>(?P<depth>#+)\s+(?P<title>.+)\s+(?P<linkmarker>\:\s*id=(?P<linkid>.*)))")
SECTION_PATTERN = re.compile(r"(?P<mdheader>(?P<depth>#+)\s+(?P<title>.+))")

INDENTED_ITEM_PATTERN = re.compile(r"^(?P<mditem>\>\s+(?P<text>.+))$", re.MULTILINE)
QUERY_ITEM_PATTERN = re.compile(r"^(?P<mditem>\?\>\s+(?P<text>.+))$", re.MULTILINE)
WARN_ITEM_PATTERN = re.compile(r"^(?P<mditem>\!\>\s+(?P<text>.+))$", re.MULTILINE)

SUMMARY_XML_ELEMENT_PATTERN = re.compile(r"(?P<mdsummary><summary>(?P<summary>.+)</summary>)")


def replace_text_in_file(file, from_text, to_text):
    file = Path(file).absolute()
    with open(file, "r") as f:
        text = f.read()
        text = text.replace(from_text, to_text)
    with open(file, "w") as f:
        f.write(text)


def is_valid_file(f):
    if not f or not f.is_file() or f.parent.stem == "ja" or f.parent.stem == "zh-cn" or f.stem == "__capabilities" or f.stem == "__langs":
        return False
    return True


# Work out the files we care about
def files_list():
    return list(filter(is_valid_file, list(sorted(docs_dir.glob("**/*.md")))))


def determine_sections(file):
    sections = []
    with open(file, "r") as f:
        for line in f.readlines():
            m = SECTION_WITH_ID_PATTERN.match(line)
            if m:
                sections.append(
                    {
                        "path": str(file.relative_to(qmk_firmware_dir / "docs")),
                        "old_section_header": m.group("mdheader"),
                        "depth": m.group("depth"),
                        "title": m.group("title"),
                        "link_id": m.group("linkid"),
                    }
                )
                continue
            m = SECTION_PATTERN.match(line)
            if m:
                sections.append(
                    {
                        "path": str(file.relative_to(qmk_firmware_dir / "docs")),
                        "old_section_header": m.group("mdheader"),
                        "depth": m.group("depth"),
                        "title": m.group("title"),
                        "link_id": None,
                    }
                )
                continue

    keyed_sections = {}

    for section in sections:
        if section["link_id"]:
            section["new_section_header"] = f"{section['depth']} {section['title']} {{#{section['link_id']}}}"
        else:
            section["new_section_header"] = f"{section['depth']} {section['title']}"
            section["link_id"] = re.sub("[^0-9a-zA-Z\-]+", "", section["title"].lower().replace(" ", "-"))
            while "--" in section["link_id"]:
                section["link_id"] = section["link_id"].replace("--", "-")

        path = section["path"]
        if path[-3:] == ".md":
            path = path[:-3]
        keyed_sections[(path, section["link_id"])] = section

    return keyed_sections


def determine_all_sections(files):
    sections = {}
    for file in files:
        sections.update(determine_sections(file))
    return sections


def replace_links(file, sections):
    file = Path(file).absolute()
    with open(file, "r") as f:
        replacements = {}
        text = f.read()
        for m in LINK_PATTERN.finditer(text):
            link = m.group("mdlink")
            label = m.group("txt")
            url = m.group("url")
            link_id = None

            # If we have a direct link to a full URL, figure out the relative path
            if url.startswith("http://docs.qmk.fm/") or url.startswith("https://docs.qmk.fm/"):
                url, anchor = (url.split("#", 2) + [None])[:2]
                if anchor and anchor[0] == "/":
                    url = anchor[1:]  # strip the leading / if present

            # Split up the URL and link ID, if present
            if "#" in url:
                url, link_id = (url.split("#", 2) + [None])[:2]

            if "?id=" in url:
                url, link_id = (url.split("?id=", 2) + [None])[:2]

            # Remove any trailing `.md`
            if url[-3:] == ".md":
                url = url[:-3]

            # Remove any `../` from the URL as we're going to manually search for the relative path anyway
            while "../" in url:
                url = url.replace("../", "")

            ###### TESTING
            if url == "20230827":
                pass

            candidate_files = list(filter(is_valid_file, list((qmk_firmware_dir / "docs").rglob(f"{url}.md"))))
            if len(candidate_files) > 1:
                raise ValueError(f"Multiple files found for {url}: {candidate_files}")
            elif len(candidate_files) > 0:
                test_file = candidate_files[0]
                if test_file is not None and test_file.exists():
                    url = os.path.relpath(test_file, file.parent)  # pathlib Path.relative_to() doesn't work here?

            # Remove any trailing `.md`...again
            if url[-3:] == ".md":
                url = url[:-3]

            if (url, link_id) in sections:
                new_link = f"[{label}]({url}#{link_id})"
            elif link_id is not None:
                new_link = f"[{label}]({url}#{link_id})"
            else:
                new_link = f"[{label}]({url})"
            if new_link != link:
                replacements[link] = new_link

        for old, new in replacements.items():
            text = text.replace(old, new)

    with open(file, "w") as f:
        f.write(text)

    pass


def replace_all_links(files, sections):
    for file in files:
        replace_links(file, sections)


def replace_sections(file, sections):
    file = Path(file).absolute()
    with open(file, "r") as f:
        text = f.read()
        for section in sections.values():
            text = text.replace(section["old_section_header"], section["new_section_header"])
    with open(file, "w") as f:
        f.write(text)


def replace_all_sections(files, sections):
    for file in files:
        replace_sections(file, sections)


def replace_image_prefixes(file):
    file = Path(file).absolute()
    with open(file, "r") as f:
        text = f.read()
        for m in IMAGE_ELEMENT_PREFIX_PATTERN.finditer(text):
            if not m.group("url").startswith("http") and not m.group("url").startswith("."):
                # Work out where the actual file is, including the relative path to the file linking to it
                image = (qmk_firmware_dir / "docs" / m.group("url")).absolute()
                rel_path = os.path.relpath(image, file.parent)

                # If it's not in a parent directory, then slap on a `./` to make it relative (vitepress requires it)
                if not str(rel_path).startswith("."):
                    rel_path = f"./{rel_path}"

                # Replace the image prefix with the relative path
                preamble = m.group("preamble")
                text = text.replace(m.group("imgprefix"), f'<img{preamble}src="{rel_path}"')
    with open(file, "w") as f:
        f.write(text)


def replace_all_image_prefixes(files):
    for file in files:
        replace_image_prefixes(file)


def replace_indented_items(file):
    file = Path(file).absolute()
    with open(file, "r") as f:
        text = f.read()
        for m in INDENTED_ITEM_PATTERN.finditer(text):
            text = text.replace(m.group("mditem"), f"::: info\n{m.group('text')}\n:::")
        for m in QUERY_ITEM_PATTERN.finditer(text):
            text = text.replace(m.group("mditem"), f"::: tip\n{m.group('text')}\n:::")
        for m in WARN_ITEM_PATTERN.finditer(text):
            text = text.replace(m.group("mditem"), f"::: warning\n{m.group('text')}\n:::")
    with open(file, "w") as f:
        f.write(text)


def replace_all_indented_items(files):
    for file in files:
        replace_indented_items(file)


def replace_details_block(file, lines, line_index):
    depth = 3
    first_line = line_index
    i = line_index + 1
    summary = ""
    while i < len(lines):
        # Start of another nested details block
        if lines[i].strip() == "<details>":
            lines, next_index, d = replace_details_block(file, lines, i)
            i = next_index
            if depth <= d:
                depth = d + 1

        # If we encounter an internal container, increment the depth so that it's considered higher in the hierarchy
        elif lines[i].strip().startswith(":::"):
            s = lines[i].strip()
            l = len(s) - len(s.lstrip(":"))
            if depth <= l:
                depth = l + 1

        # If we got a summary, make sure we remember the text
        elif SUMMARY_XML_ELEMENT_PATTERN.match(lines[i].strip()):
            summary = " " + SUMMARY_XML_ELEMENT_PATTERN.match(lines[i].strip()).group("summary")
            del lines[i]
            i -= 1

        # End of the details block
        elif lines[i].strip() == "</details>":
            last_line = i
            break

        i += 1

    lines[first_line] = (depth * ":") + f" details{summary}"
    lines[last_line] = depth * ":"
    return lines, i, depth


def replace_details_blocks(file):
    file = Path(file).absolute()
    with open(file, "r") as f:
        lines = f.read().split("\n")
        i = 0
        while i < len(lines):
            if lines[i].strip() == "<details>":
                lines, next_index, _ = replace_details_block(file, lines, i)
                i = next_index
            i += 1
    with open(file, "w") as f:
        f.write("\n".join(lines))


def replace_all_details_blocks(files):
    for file in files:
        replace_details_blocks(file)


TABS_START_PATTERN = re.compile(r"^(?P<mdtabs>\<!--\s*tabs\s*:\s*start\s*--\>)", re.IGNORECASE)
TABS_END_PATTERN = re.compile(r"^(?P<mdtabs>\<!--\s*tabs\s*:\s*end\s*--\>)", re.IGNORECASE)
TAB_TITLE_PATTERN = re.compile(r"^(?P<mdtabtitle>#+\s*\*\*\s+(?P<title>.*)\s*\*\*)", re.IGNORECASE)


def replace_tabs_block(file, lines, line_index):
    pass


def replace_tabs_blocks(file):
    file = Path(file).absolute()
    with open(file, "r") as f:
        lines = f.read().split("\n")
        i = 0
        while i < len(lines):
            if TABS_START_PATTERN.match(lines[i]):
                lines, next_index, _ = replace_tabs_block(file, lines, i)
                i = next_index
            i += 1
    with open(file, "w") as f:
        f.write("\n".join(lines))


def replace_all_tabs_blocks(files):
    for file in files:
        replace_tabs_blocks(file)


# -----------------------------------------------------------------------------------------------------------------------

# Make sure we're on the `vitepress` branch
os.chdir(qmk_firmware_dir)
if _run("git branch --show-current").stdout.strip() != "vitepress":
    print(
        "Please switch to the vitepress branch before running this script.",
        file=sys.stderr,
    )
    sys.exit(1)

# Revert docs files to current state of `develop`, apart from the new capabilities page.
_run("git checkout develop -- docs")

# Remove internals directory as it's basically rotting, along with any other leftovers from the docsify implementation
remove_files = [
    "docs/_langs.md",
    "docs/_summary.md",
    "docs/.nojekyll",
    "docs/CNAME",
    "docs/index.html",
    "docs/internals",
    "docs/ja",
    "docs/qmk*.css",
    "docs/README.md",
    "docs/redirects.json",
    "docs/sw.js",
    "docs/translating.md",
    "docs/zh-cn",
]
for file in remove_files:
    _run(f"git rm -rf {file}")

# Reinstate files that have been modified on the `vitepress` branch
restore_files = [
    "docs/__capabilities.md",
    "docs/hardware_keyboard_guidelines.md",
]
for file in restore_files:
    _run(f"git checkout vitepress -- {file}")

# Clear out any staged changes
_run("git reset")

files = files_list()
sections = determine_all_sections(files)
replace_all_links(files, sections)
replace_all_sections(files, sections)
replace_all_image_prefixes(files)
replace_all_indented_items(files)
replace_all_details_blocks(files)

# Random fixes

replace_text_in_file(
    qmk_firmware_dir / "docs" / "breaking_changes.md",
    "<2 Days Before>",
    "`<2 Days Before>`",
)
replace_text_in_file(
    qmk_firmware_dir / "docs" / "breaking_changes.md",
    "<Day of Merge>",
    "`<Day of Merge>`",
)

replace_text_in_file(
    qmk_firmware_dir / "docs" / "coding_conventions_python.md",
    "[setup.cfg](setup.cfg)",
    "[setup.cfg](https://github.com/qmk/qmk_firmware/blob/master/setup.cfg)",
)

replace_text_in_file(
    qmk_firmware_dir / "docs" / "faq_build.md",
    "[Build Environment Setup](getting_started_build_tools)",
    "[Build Environment Setup](newbs_getting_started)",
)

replace_text_in_file(qmk_firmware_dir / "docs" / "feature_stenography.md", "```mk", "```make")

replace_text_in_file(
    qmk_firmware_dir / "docs" / "newbs.md",
    "[ask us for guidance](getting_started_getting_help)",
    "[ask us for guidance](support)",
)

replace_text_in_file(
    qmk_firmware_dir / "docs" / "serial_driver.md",
    '["Alternate Functions for selected STM32 MCUs"](alternate-functions-for-selected-stm32-mcus)',
    '["Alternate Functions for selected STM32 MCUs"](#alternate-functions-for-selected-stm32-mcus)',
)
