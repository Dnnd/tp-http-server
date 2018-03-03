FROM base/archlinux

RUN pacman -Sy cmake clang --noconfirm

EXPOSE 80
