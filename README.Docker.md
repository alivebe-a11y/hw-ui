# Running hw-ui in Docker (TrueNAS SCALE / Nvidia)

This repo ships a `Dockerfile` and `docker-compose.yml` so `hw-smi` can be run
as an interactive TUI inside a container, targeting **TrueNAS SCALE 24.10+
(Electric Eel)** with an **Nvidia GPU**.

## Prerequisites
- TrueNAS SCALE 24.10+ with native Docker + `docker compose`.
- Nvidia driver enabled via **System Settings → Advanced → GPU → Install
  NVIDIA Drivers**. TrueNAS auto-configures `nvidia-container-toolkit`,
  which mounts `libnvidia-ml.so.1` and `/dev/nvidia*` into containers that
  request a GPU.
- x86_64 host. The `Dockerfile` is x86_64-only.

## Build
```bash
git clone https://github.com/alivebe-a11y/hw-ui.git
cd hw-ui
docker compose build
```

Build log should show `Info: Nvidia GPU driver found!` and
`Compiling for Nvidia GPUs:`.

## Run
```bash
docker compose run --rm hw-smi             # default view
docker compose run --rm hw-smi --graphs    # line-graph view
docker compose run --rm hw-smi --bars      # bar view
docker compose run --rm hw-smi --help      # usage
```
Exit with Ctrl+C.

## Notes
- No sudo needed inside the container (Nvidia NVML does not require it).
- UTF-8 locale is set so the block glyphs render correctly.
- `NVIDIA_DRIVER_CAPABILITIES=utility` is the minimum needed for NVML.
- Fallback for older Docker Compose:
  ```yaml
  runtime: nvidia
  environment:
    NVIDIA_VISIBLE_DEVICES: all
    NVIDIA_DRIVER_CAPABILITIES: utility
  ```

## Unsupported
- AMD / Intel GPUs (not built into this image).
- TrueNAS CORE (FreeBSD, no native Linux Docker).
- aarch64 hosts (change the stub symlink path + base image).
