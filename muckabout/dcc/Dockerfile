ARG OSXCROSS_VERSION=latest
FROM crazymax/osxcross:${OSXCROSS_VERSION}-alpine AS osxcross
FROM --platform=$BUILDPLATFORM tonistiigi/xx:1.1.2 AS xx

FROM --platform=$BUILDPLATFORM alpine
COPY --from=xx / /
RUN apk add --no-cache bash clang lld musl-dev
ARG TARGETPLATFORM
RUN xx-apk add gcc g++ musl-dev
WORKDIR /work
RUN --mount=type=bind,source=.,target=/work \
    --mount=type=bind,from=osxcross,source=/osxsdk,target=/xx-sdk \
    bash
