FROM niessan/omnetpp-inet:omnet-5.4.2-inet-4.1.0

RUN mkdir -p /root/models/nesting
WORKDIR /root/models/nesting
COPY . /root/models/nesting
RUN make makefiles && make clean && make -j$(grep -c proc /proc/cpuinfo) && make MODE=debug -j$(grep -c proc /proc/cpuinfo)
ENV INET=/root/models/inet NESTING=/root/models/nesting OMNETPP=/root/omnetpp
