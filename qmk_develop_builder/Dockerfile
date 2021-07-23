FROM qmkfm/base_container

RUN apt-get update \
    && apt-get install -y awscli remake curl gawk ccache busybox

RUN groupadd -f qmk -g 10000 \
    && useradd -d /home/qmk -g qmk -m -N -u 10000 qmk

RUN mkdir -p /usr/local/bin \
    && curl -L https://github.com/pixelb/scripts/raw/master/scripts/ansi2html.sh > /usr/local/bin/ansi2html.sh \
    && chmod 755 /usr/local/bin/ansi2html.sh

USER 10000:10000
WORKDIR /home/qmk

RUN python3 -m pip install -U pip wheel \
    && python3 -m pip install -U qmk milc \
    && git clone -b develop https://github.com/qmk/qmk_firmware /home/qmk/qmk_firmware \
    && cd /home/qmk/qmk_firmware \
    && git checkout develop \
    && git pull --ff-only \
    && make git-submodule

COPY entrypoint.sh /home/qmk/entrypoint.sh
COPY build_all.sh /home/qmk/build_all.sh

USER 0:0
WORKDIR /home/qmk

RUN chmod 755 /home/qmk/*.sh \
    && chown qmk:qmk /home/qmk/*.sh

USER 10000:10000
WORKDIR /home/qmk

ENTRYPOINT ["/home/qmk/entrypoint.sh"]