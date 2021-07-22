#!/bin/bash

set -eEuo pipefail

# Allow for other user accounts to delete the generated files, if we're mounting into the docker container somewhere...
umask 000

unset AWS_KEY
unset AWS_SECRET
unset AWS_BUCKET
unset RUN_SHELL
unset DISCORD_WEBHOOK
while [[ -n "${1:-}" ]] ; do
    case "$1" in
        --key)
            shift;
            AWS_KEY=$1;
            ;;
        --secret)
            shift;
            AWS_SECRET=$1;
            ;;
        --bucket)
            shift;
            AWS_BUCKET=$1;
            ;;
        --shell)
            RUN_SHELL=1;
            ;;
        --discord)
            shift;
            DISCORD_WEBHOOK=$1;
            ;;
        *)
            echo "Unknown arg '$1'." >&2
            exit 1
            ;;
    esac
    shift
done

if [[ -z "${AWS_KEY-}" ]] ; then
    echo "Missing '--key <AWS_KEY>' argument" >&2
    exit 1
fi

if [[ -z "${AWS_SECRET-}" ]] ; then
    echo "Missing '--secret <AWS_SECRET>' argument" >&2
    exit 1
fi

if [[ -z "${AWS_BUCKET-}" ]] ; then
    echo "Missing '--bucket <AWS_BUCKET>' argument" >&2
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
}

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
        rm -rf /home/qmk/qmk_firmware/.build/*
        env -i HOME="$HOME" PATH="/usr/lib/ccache:/usr/local/bin:/usr/bin:/bin" TERM="linux" PWD="${PWD:-}" /home/qmk/build_all.sh | sort || true
    } 2>&1 > /home/qmk/qmk_build_all.log
}

summary() {
    num_successes=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(OK)\]' | wc -l)
    num_skipped=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(SKIPPED)\]' | wc -l)
    num_warnings=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(WARNINGS)\]' | wc -l)
    num_failures=$(cat /home/qmk/qmk_build_all.log | grep -E '\[(ERRORS)\]' | wc -l)
    echo "Successful builds: $num_successes"
    echo "Skipped builds: $num_skipped"
    echo "Warning builds: $num_warnings"
    echo "Failing builds: $num_failures"
    cat /home/qmk/qmk_build_all.log | grep -E '\[(ERRORS)\]'
}

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
    if [[ ! -z "${DISCORD_WEBHOOK:-}" ]] ; then
        local discord_output="$(discord_text)"
        curl \
            -H "Content-Type: application/json" \
            -d "{\"username\": \"QMK Develop Builder\", \"content\": \"${discord_output//$'\n'/\\n}\"}" \
            "${DISCORD_WEBHOOK}"
    fi
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
h1, h2, pre { font-family: 'Iosevka Term', 'Iosevka Fixed', Consolas, Menlo, 'Courier New', monospace; }
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
$(git log -n5 --color=always | ctlchars2html)
</pre>
<hr/>
<pre>
$(summary | ctlchars2html)
</pre>
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
</body></html>
EOF
    } 2>&1 > /home/qmk/index.html
}

upload_binaries() {
    aws s3 cp /home/qmk/qmk_firmware/ s3://${AWS_BUCKET}/ --recursive --exclude '*' --include '*.bin' --include '*.uf2' --include '*.hex' --exclude '*/*'
    aws s3 cp /home/qmk/index.html s3://${AWS_BUCKET}/
    aws s3 ls s3://${AWS_BUCKET}/
}

python3 -m pip install -U qmk milc

get_qmk

if [[ -z "${RUN_SHELL:-}" ]] ; then

    cd /home/qmk/qmk_firmware

    build_qmk
    make_index_html

    save_aws_creds
    clear_s3_bucket
    upload_binaries
    rm -f "$HOME/.aws/credentials" || true

    send_discord

else

    exec bash

fi