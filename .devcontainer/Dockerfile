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
      python3-pip \
      python3-venv \
      sudo \
 && rm -rf /var/lib/apt/lists/*

COPY requirements-docs.txt /tmp/requirements-docs.txt
RUN python3 -m venv /opt/docs-venv \
 && /opt/docs-venv/bin/pip install --no-cache-dir -r /tmp/requirements-docs.txt \
 && rm /tmp/requirements-docs.txt
ENV PATH="/opt/docs-venv/bin:${PATH}"

RUN useradd -m -s /bin/bash vscode \
 && usermod -aG sudo vscode \
 && echo 'vscode ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/vscode \
 && mkdir -p /workspaces/Cppuccino \
 && chown vscode:vscode /workspaces/Cppuccino

USER vscode
WORKDIR /workspaces/Cppuccino
