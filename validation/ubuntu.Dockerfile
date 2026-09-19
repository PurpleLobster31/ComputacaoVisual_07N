# Executa Linux real em um conteiner; nao representa Windows/WSL nem a GPU fisica.
FROM ubuntu:26.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc-15 g++-15 make pkg-config cmake ninja-build curl ca-certificates \
    libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev \
    libpng-dev libjpeg-dev libfreetype-dev libharfbuzz-dev \
    xvfb xauth openbox dbus-x11 x11-utils && rm -rf /var/lib/apt/lists/*
ENV CC=gcc-15 CXX=g++-15
# Mesmas versoes SDL utilizadas no Mac; builds a partir dos releases oficiais.
RUN curl -fL https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz -o /tmp/sdl.tar.gz \
 && tar -xf /tmp/sdl.tar.gz -C /tmp \
 && cmake -S /tmp/SDL3-3.4.16 -B /tmp/sdl-build -G Ninja -DCMAKE_BUILD_TYPE=Release -DSDL_TEST_LIBRARY=OFF \
 && cmake --build /tmp/sdl-build -j2 && cmake --install /tmp/sdl-build
RUN curl -fL https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.6/SDL3_image-3.4.6.tar.gz -o /tmp/image.tar.gz \
 && tar -xf /tmp/image.tar.gz -C /tmp \
 && cmake -S /tmp/SDL3_image-3.4.6 -B /tmp/image-build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF \
 && cmake --build /tmp/image-build -j2 && cmake --install /tmp/image-build
RUN curl -fL https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz -o /tmp/ttf.tar.gz \
 && tar -xf /tmp/ttf.tar.gz -C /tmp \
 && cmake -S /tmp/SDL3_ttf-3.2.2 -B /tmp/ttf-build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local -DSDLTTF_SAMPLES=OFF \
 && cmake --build /tmp/ttf-build -j2 && cmake --install /tmp/ttf-build \
 && ldconfig && rm -rf /tmp/*
ENV CC=gcc-15 SDL_VIDEODRIVER=x11
WORKDIR /project
CMD ["bash"]
