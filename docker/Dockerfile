ARG BASE_VERSION=22.04

FROM ubuntu:${BASE_VERSION} AS riscv-builder

ENV RISCV="/risc-v"

RUN apt-get update \
    && apt-get install -y \
    autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev \
    libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc \
    zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev device-tree-compiler

RUN <<EOF
    set -eux
    mkdir -p $RISCV
    cd /opt
    # NOTE: Has been moved to riscv-collab, but its URL of submodules are hardcoded.
    # Updating the link would break the submodule, so we retained the old link.
    git clone -b rvv-0.8 --recurse-submodules --shallow-submodules --depth=1 https://github.com/riscv/riscv-gnu-toolchain
    cd riscv-gnu-toolchain
    ./configure --prefix=$RISCV --with-arch=rv32gc --with-abi=ilp32d
    make -j$(nproc)
    make clean
EOF

ENV PATH="$RISCV/bin:$PATH"

RUN <<EOF
    set -eux
    cd /opt
    git clone --depth=1 https://github.com/riscv-software-src/riscv-pk.git
    cd riscv-pk
    mkdir build && cd build
    ../configure --prefix=$RISCV --host=riscv32-unknown-elf
    make -j$(nproc)
    make -j$(nproc) install
    make clean
EOF

RUN <<EOF
    set -eux
    cd /opt
    # NOTE: Newer versions seem to poll the stdin, leading to unexpected errors.
    git clone -b v1.1.0 --depth=1 https://github.com/riscv-software-src/riscv-isa-sim.git
    cd riscv-isa-sim
    mkdir build && cd build
    ../configure --prefix=$RISCV
    make -j$(nproc)
    make -j$(nproc) install
    make clean
EOF

# final image
FROM ubuntu:${BASE_VERSION}

RUN apt-get update \
    && apt-get --no-install-recommends install -y \
    gosu \
    python3 python3-pip python3-setuptools python3-wheel \
    build-essential make \
    gdb \
    flex libfl-dev \
    bison libbison-dev \
    device-tree-compiler \
    # will be installed at /usr/local/lib/python3.10/dist-packages
    && pip3 install colorama \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
    && apt-get --no-install-recommends install -y \
    dfu-util \
    # will be installed at /usr/local/lib/python3.10/dist-packages
    && pip3 install platformio \
    && rm -rf /var/lib/apt/lists/*

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# optional packages
ARG CONTAINER_USERNAME=student
ARG CONTAINER_GROUPNAME=studentg
ARG CONTAINER_HOMEDIR=/home/student

ENV DOCKER_USERNAME_PASSIN ${CONTAINER_USERNAME}
ENV DOCKER_GROUPNAME_PASSIN ${CONTAINER_GROUPNAME}
ENV DOCKER_HOMEDIR_PASSIN ${CONTAINER_HOMEDIR}

# flag to prevent student to recursively enter docker environment
ENV STATUS_DOCKER_ACTIVATED DOCKER_ACTIVATED

COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENV RISCV="/risc-v"
COPY --link --from=riscv-builder /risc-v/ /risc-v/
ENV PATH="$RISCV/bin:$PATH"

CMD ["/bin/bash"]
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
