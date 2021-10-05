.PHONY: default
default: test

.PHONY: test
test: init
	./vendor/bin/phpunit --testdox src/tests

.PHONY: init
init: githooks

.PHONY: githooks
githooks:
	@if command -v git &> /dev/null; then \
		git config core.hooksPath .githooks; \
	fi
