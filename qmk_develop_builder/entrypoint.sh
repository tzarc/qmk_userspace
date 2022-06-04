#!/bin/bash

set -eEuo pipefail

# Allow for other user accounts to delete the generated files, if we're mounting into the docker container somewhere...
umask 000

########################################################################################################################################################################################################
# If we have an AWS bucket, then ensure we have credentials
if [[ ! -z "${AWS_BUCKET-}" ]] ; then

    if [[ -z "${AWS_KEY-}" ]] ; then
        echo "Missing '--key <AWS_KEY>' argument" >&2
        exit 1
    fi

    if [[ -z "${AWS_SECRET-}" ]] ; then
        echo "Missing '--secret <AWS_SECRET>' argument" >&2
        exit 1
    fi

save_aws_creds() {
    [[ -d "$HOME/.aws" ]] || mkdir -p "$HOME/.aws"
    cat << EOF > "$HOME/.aws/credentials"
[default]
aws_access_key_id = ${AWS_KEY}
aws_secret_access_key = ${AWS_SECRET}
EOF
}

clear_s3_bucket() {
    aws s3 rm --recursive s3://${AWS_BUCKET}/
    aws s3 ls s3://${AWS_BUCKET}/

        cd /home/qmk/qmk_firmware
    cat << EOF > /home/qmk/in_progress.html
<!DOCTYPE html>
<html lang='en'><head>
<meta charset="utf-8"/>
<style type='text/css'>
$(ansi2html.sh --bg=dark --palette=linux --css-only 2>/dev/null)
pre { font-size: 80%; }
h1, h2, h3, pre { font-family: 'Iosevka Term', 'Iosevka Fixed', Consolas, Menlo, 'Courier New', monospace; }
a { color: #FF0; font-weight: bold; }
a:visited { color: #FF0; }
a:hover { color: #F00; }
</style>
<title>qmk_firmware develop @ $(git log -n1 --format=format:%H)</title>
</head><body class='f9 b9'>
<div style='float:left'>
<h3>qmk_firmware develop @ $(git log -n1 --format=format:%H)</h3>
Upload in progress.
</div>
</body>
</html>
EOF

    aws s3 cp /home/qmk/in_progress.html s3://${AWS_BUCKET}/index.html
}

upload_binaries() {
    aws s3 cp /home/qmk/qmk_firmware/ s3://${AWS_BUCKET}/ --recursive --exclude '*' --include '*.bin' --include '*.uf2' --include '*.hex' --exclude '*/*'
    aws s3 cp /home/qmk/qmk_firmware/.build/ s3://${AWS_BUCKET}/ --recursive --exclude '*' --include 'failed.*.html' --exclude '*/*'
    aws s3 cp /home/qmk/index.html s3://${AWS_BUCKET}/
    aws s3 ls s3://${AWS_BUCKET}/
}

fi

########################################################################################################################################################################################################
# If we have a Discord hook, sort out the discord functions

if [[ ! -z "${DISCORD_WEBHOOK-}" ]] ; then

discord_text() {
    {
        local num_successes=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(OK)\]' | wc -l)
        local num_warnings=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(WARNINGS)\]' | wc -l)
        local num_failures=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(ERRORS)\]' | wc -l)
        echo "qmk_firmware, develop @ \`$(cd /home/qmk/qmk_firmware && git log -n1 --format=format:%H)\`"
        echo "Prebuilt firmware at https://qmk.tzarc.io/"
        echo "Successful: **${num_successes}**, warnings: **${num_warnings}**, errors: **${num_failures}**"
        echo '```'
        local ERRORS=$(cat /home/qmk/qmk_build_all.log | grep -P '^Build ' | grep '\[ERRORS\]' | sort | awk '{printf "  %s\n",$2}')
        if [[ ! -z "$ERRORS" ]] ; then
            echo 'Failing builds:'
            echo "$ERRORS"
        else
            echo "  No failing builds."
        fi
        echo '```'
    } | sed -e 's@"@\"@g' -e 's@\n@\\n@g'
}

send_discord() {
        local discord_output="$(discord_text)"
        local discord_output_cut=${discord_output:0:1900}
        if [[ $discord_output != $discord_output_cut ]] ; then
            discord_output_cut="$(echo ${discord_output_cut%%\`}; echo; echo "<<snip>>\`\`\`")"
        fi
        curl \
            -H "Content-Type: application/json" \
            -d "{\"username\": \"QMK Develop Builder\", \"content\": \"${discord_output_cut//$'\n'/\\n}\"}" \
            "${DISCORD_WEBHOOK}"
}

fi

########################################################################################################################################################################################################
# Helpers

ctlchars2html() {
    cat - | ansi2html.sh --bg=dark --palette=linux --body-only 2>/dev/null
}

get_qmk() {
    {
        cd /home/qmk/qmk_firmware
        git checkout develop
        git pull --ff-only
        make git-submodule
    } 2>&1 > /home/qmk/qmk_get.log
}

build_qmk() {
    {
        cd /home/qmk/qmk_firmware
        env -i HOME="$HOME" PATH="/home/qmk/.local/bin:/usr/local/bin:/usr/bin:/bin" TERM="linux" PWD="${PWD:-}" /home/qmk/run_ci_build.sh | sort || true
    } 2>&1 > /home/qmk/qmk_build_all.log

#    {
#        cd /home/qmk/qmk_firmware
#        { qmk list-keyboards | grep handwired/onekey | while read kb ; do fmake ${kb}:all 2>&1 ; done | grep Making ; } || true
#    } 2>&1 > /home/qmk/qmk_build_onekey.log
}

summary() {
    local logfile="$1"
    num_successes=$(cat "$logfile" | grep -E '\[(OK)\]' | wc -l)
    num_skipped=$(cat "$logfile" | grep -E '\[(SKIPPED)\]' | wc -l)
    num_warnings=$(cat "$logfile" | grep -E '\[(WARNINGS)\]' | wc -l)
    num_failures=$(cat "$logfile" | grep -E '\[(ERRORS)\]' | wc -l)
    echo "Successful builds: $num_successes"
    echo "Skipped builds: $num_skipped"
    echo "Warning builds: $num_warnings"
    echo "Failing builds: $num_failures"
    cat "$logfile" | grep -E '\[(ERRORS)\]'
}

failure_output() {
    echo "<h3>Failure logs:</h3>"

    { ls -1 /home/qmk/qmk_firmware/.build/failed* 2>/dev/null || true ; } | sort | while read failure ; do
        echo "<hr/>"
        echo "<h3>$(echo $failure | rev | cut -d. -f1 | rev)</h3>"
        echo "<pre>"
        cat $failure | ctlchars2html
        echo "</pre>"
    done
}

make_index_html() {
    {
        cd /home/qmk/qmk_firmware
        cat << EOF > /home/qmk/index.html
<!DOCTYPE html>
<html lang='en'><head>
<meta charset="utf-8"/>
<style type='text/css'>
$(ansi2html.sh --bg=dark --palette=linux --css-only 2>/dev/null)
pre { font-size: 80%; }
h1, h2, h3, pre { font-family: 'Iosevka Term', 'Iosevka Fixed', Consolas, Menlo, 'Courier New', monospace; }
a { color: #FF0; font-weight: bold; }
a:visited { color: #FF0; }
a:hover { color: #F00; }
</style>
<title>qmk_firmware develop @ $(git log -n1 --format=format:%H)</title>
</head><body class='f9 b9'>
<div style='float:left'>
<h1>QMK develop branch build</h1>
<h2>Commit: ($(git log -n1 --format=format:%H))</h2>
<h2>Build date: $(date -u)</h2>
<hr/>
<pre>
$(git log -n5 --color=always --no-merges | ctlchars2html)
</pre>
<hr/>
<pre>
$(summary /home/qmk/qmk_build_all.log | ctlchars2html)
</pre>
<hr/>
$(failure_output)
<hr/>
<pre>
$(cat /home/qmk/qmk_get.log | ctlchars2html)
</pre>
<hr/>
<pre>
$(cat /home/qmk/qmk_build_all.log | ctlchars2html)
</pre>
</div>
<div style='position:absolute; right:0; top:0; padding: 1em; border-left: 1px solid #666; border-bottom: 1px solid #666' class="f9 b9">
<pre>
Prebuilt binaries:
$(cd /home/qmk/qmk_firmware/ >/dev/null 2>&1; for f in $(ls *.hex *.bin *.uf2 2>/dev/null) ; do
    echo "<a href='$f'>$f</a>"
done)
</pre>
</div>
<!--
<div style='float:left'>
<h1>QMK develop branch OneKey builds</h1>
<pre>
$(summary /home/qmk/qmk_build_onekey.log | ctlchars2html)
</pre>
</div>
-->
</body></html>
EOF
    } 2>&1 > /home/qmk/index.html
}

python3 -m pip install -U qmk >/dev/null 2>&1

get_qmk >/dev/null 2>&1

if [[ "${1:-}" != "--shell" ]] ; then

    cd /home/qmk/qmk_firmware

    hash_file="/home/qmk/.repo-hash"
    old_hash=$(cat "$hash_file" || true)
    new_hash=$(git log -n1 --format=format:%H)

    if [[ "$old_hash" == "$new_hash" ]] ; then
        exit 0
    fi

    echo $new_hash > "$hash_file"
    build_qmk
    make_index_html

    if [[ ! -z "${AWS_BUCKET-}" ]] ; then
        save_aws_creds
        clear_s3_bucket
        upload_binaries
        rm -f "$HOME/.aws/credentials" || true
    fi

    if [[ ! -z "${DISCORD_WEBHOOK:-}" ]] ; then
        send_discord
    fi

else

    exec bash

fi