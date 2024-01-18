# XXX: We were supposed to support gcc/g++11 using Ubuntu 22.04.
# However, the old image references a branch/tag of the riscv-gnu-toolchain repository that no longer exists:
# git clone -b rvv-0.7.x --recursive https://github.com/riscv/riscv-gnu-toolchain.
# We cannot simply rebuild the image with the base image of Ubuntu 22.04,
# and upgrading it slightly to rvv-0.8 breaks the test cases.
# To still provide the support, gcc-11 and g++-11 are installed manually and linked to /usr/bin/gcc and /usr/bin/g++.

FROM ubuntu:18.04 AS riscv-builder

ENV RISCV /risc-v

RUN apt-get update

RUN apt-get install -y \
    git autoconf automake autotools-dev curl \
    libmpc-dev libmpfr-dev libgmp-dev libusb-1.0-0-dev gawk \
    build-essential  device-tree-compiler bison flex \
    texinfo gperf libtool patchutils bc zlib1g-dev \
    device-tree-compiler pkg-config libexpat-dev

RUN mkdir -p $RISCV && cd ~ && \
    git clone -b rvv-0.7.x --recursive https://github.com/riscv/riscv-gnu-toolchain && \
    cd riscv-gnu-toolchain && ./configure --prefix=$RISCV --with-arch=rv32gc --with-abi=ilp32d && \
    make -j "$(nproc)" && make clean

ENV PATH="/risc-v/bin:${PATH}"

RUN cd ~ && git clone https://github.com/riscv/riscv-pk.git && \
    cd riscv-pk && mkdir build && cd build && \
    ../configure --prefix=$RISCV --host=riscv32-unknown-elf && \
    make -j "$(nproc)" && make install -j "$(nproc)" && make clean

RUN cd ~ && git clone https://github.com/riscv/riscv-isa-sim.git && \
    cd riscv-isa-sim && mkdir build && cd build && \
    ../configure --prefix=$RISCV && make -j "$(nproc)" && make install -j "$(nproc)" && make clean


# final image
FROM ubuntu:18.04

ENV RISCVBIN /risc-v/bin
ENV RISCV /risc-v
COPY --from=riscv-builder /risc-v/bin/riscv32-unknown-elf-gcc /risc-v/bin/spike /risc-v/bin/   
COPY --from=riscv-builder /risc-v/lib/ /risc-v/lib/ 
COPY --from=riscv-builder /risc-v/libexec/ /risc-v/libexec/
COPY --from=riscv-builder /risc-v/riscv32-unknown-elf/ /risc-v/riscv32-unknown-elf/
# COPY --from=riscv-builder /risc-v/ /risc-v/

ENV PATH="/risc-v/bin:${PATH}"

RUN apt-get update && apt-get install -y \
    gosu \
    bison flex \
    device-tree-compiler \
    python3 python3-pip \
    dfu-util

RUN pip3 install platformio
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update \
    && apt-get --no-install-recommends install -y \
    gosu \
    python3 python3-pip python3-setuptools python3-wheel \
    build-essential make \
    gdb \
    flex libfl-dev \
    bison libbison-dev \
    # will be installed at /usr/local/lib/python3.10/dist-packages
    && pip3 install colorama \
    # now we are to replace the old gcc/g++ version 7 with version 11
    && apt-get remove -y gcc g++ \
    && apt-get install -y software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    # NOTE: using update-alternatives may be a better way
    && apt-get install -y gcc-11 g++-11 \
    && ln -s $(which gcc-11) /usr/bin/gcc \
    && ln -s $(which g++-11) /usr/bin/g++ \
    && rm -rf /var/lib/apt/lists/*

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

CMD ["/bin/bash"]
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
