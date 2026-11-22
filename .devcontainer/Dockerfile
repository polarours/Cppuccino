# Cppuccino development environment: the same checks as CI, plus a modern
# toolchain for the C++23/C++26 examples.
FROM gcc:15

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      ca-certificates \
      clang \
      cmake \
      git \
      ninja-build \
      python3 \
      sudo \
 && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash vscode \
 && usermod -aG sudo vscode \
 && echo 'vscode ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/vscode \
 && mkdir -p /workspaces/Cppuccino \
 && chown vscode:vscode /workspaces/Cppuccino

USER vscode
WORKDIR /workspaces/Cppuccino
