# syntax=docker/dockerfile:1.6

# ---- builder: compile hw-smi against the NVML stub ----
FROM nvidia/cuda:12.4.1-devel-ubuntu22.04 AS builder

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        g++ \
        ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . /src

# make.sh searches a fixed list of filesystem paths for libnvidia-ml.so and
# passes the match as a positional argument to g++. The real libnvidia-ml is
# injected at runtime by nvidia-container-toolkit, so at build time we link
# against the stub shipped with the CUDA devel image.
RUN ln -sf /usr/local/cuda/lib64/stubs/libnvidia-ml.so \
           /usr/lib/x86_64-linux-gnu/libnvidia-ml.so \
 && chmod +x make.sh \
 && ./make.sh

# ---- runtime: small image; libnvidia-ml.so.1 is mounted by the NVIDIA runtime ----
FROM nvidia/cuda:12.4.1-base-ubuntu22.04

ENV NVIDIA_VISIBLE_DEVICES=all \
    NVIDIA_DRIVER_CAPABILITIES=utility \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8 \
    TERM=xterm-256color

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        libstdc++6 \
        locales \
 && locale-gen C.UTF-8 \
 && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/bin/hw-smi /usr/local/bin/hw-smi

ENTRYPOINT ["/usr/local/bin/hw-smi"]
