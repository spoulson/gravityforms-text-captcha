VERSION := $(shell git symbolic-ref -q --short HEAD)
BUILD_ARTIFACT := gravityforms-text-captcha-${VERSION}.tar.gz
.DEFAULT_GOAL := test

.PHONY: lint
lint: init
	./src/vendor/bin/phplint

.PHONY: test
test: init
	./src/vendor/bin/phpunit

.PHONY: build
build: init
	composer install --no-dev
	find . -type d -maxdepth 1 -name build -exec rm -rf {} \;
	mkdir -p build/gravityforms-text-captcha
	cp -r assets/* src/include src/vendor src/gravityforms-text-captcha.php build/gravityforms-text-captcha
	tar -zcvf ${BUILD_ARTIFACT} -C build gravityforms-text-captcha
	@echo
	@echo Built artifact: ${BUILD_ARTIFACT}

.PHONY: clean
clean: init
	rm -rf build ${BUILD_ARTIFACT} src/vendor

.PHONY: init
init: githooks

.PHONY: githooks
githooks:
	@if command -v git &> /dev/null; then \
		git config core.hooksPath .githooks; \
	fi

.PHONY: start-wordpress
start-wordpress:
	docker-compose -f docker-compose-wordpress.yaml up -d

.PHONY: stop-wordpress
stop-wordpress:
	docker-compose -f docker-compose-wordpress.yaml down

.PHONY: deploy
deploy:
	tar -zxvf ${BUILD_ARTIFACT} -C wordpress/wp-content/plugins
