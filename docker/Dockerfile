FROM ubuntu:18.04

# TODO: add pip into dependency

# Combine all three RUNs into 1 in deloyment
RUN apt-get update
RUN apt-get --no-install-recommends install -y \
    python3 python3-pip python3-setuptools python3-wheel\
    gcc \
    build-essential make \
    flex libfl-dev \
    bison \
    && rm -rf /var/lib/apt/lists/*
# RUN rm -rf /var/lib/apt/lists/*


# will be installed at /usr/local/lib/python3.6/dist-packages
RUN pip3 install colorama

# flag to prevent student to recursively enter docker environment
ENV STATUS_DOCKER_ACTIVATED DOCKER_ACTIVATED

RUN sed -i \
    -e 's@\\u@$(whoami 2>/dev/null || echo ${USER-I have no name!})@' \
    -e "s@groups@groups 2>/dev/null@" \
    /etc/bash.bashrc
