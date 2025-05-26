FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
  build-essential \
  libcurl4-openssl-dev \
  libjson-c-dev \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY *.c *.h Makefile ./

RUN make release

EXPOSE 8080

CMD ["./robot_dialog_server", "-p", "8080"]