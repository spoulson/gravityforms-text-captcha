VERSION := $(shell git symbolic-ref -q --short HEAD)
BUILD_ARTIFACT := gravityforms-text-captcha-${VERSION}.tar.gz

.PHONY: default
default: test

.PHONY: test
test: init
	./vendor/bin/phpunit --testdox src/tests

.PHONY: build
build: init
	mkdir -p build
	cp -r assets/* build
	cp -r src/include build
	tar -zcvf ${BUILD_ARTIFACT} -C build .

.PHONY: init
init: githooks

.PHONY: githooks
githooks:
	@if command -v git &> /dev/null; then \
		git config core.hooksPath .githooks; \
	fi
