VERSION := $(shell git symbolic-ref -q --short HEAD)
BUILD_ARTIFACT := gravityforms-text-captcha-${VERSION}.tar.gz
.DEFAULT_GOAL := test

.PHONY: lint
lint: init
	./vendor/bin/phplint

.PHONY: test
test: init
	./vendor/bin/phpunit

.PHONY: build
build: init
	find . -type d -maxdepth 1 -name build -exec rm -rf {} \;
	mkdir -p build/gravityforms-text-captcha
	cp -r assets/* src/include src/gravityforms-text-captcha.php build/gravityforms-text-captcha
	tar -zcvf ${BUILD_ARTIFACT} -C build gravityforms-text-captcha
	@echo
	@echo Built artifact: ${BUILD_ARTIFACT}

.PHONY: init
init: githooks

.PHONY: githooks
githooks:
	@if command -v git &> /dev/null; then \
		git config core.hooksPath .githooks; \
	fi
