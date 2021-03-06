FROM php:8-alpine
RUN apk add -U wget make

# Install Composer.
RUN wget -O /root/composer-setup https://getcomposer.org/installer
RUN php /root/composer-setup --install-dir=/usr/local/bin --filename=composer

# Install dependencies.
WORKDIR /build
COPY . .
RUN composer install

ENTRYPOINT ["make"]
