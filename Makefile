.PHONY: default
default: test

.PHONY: test
test:
	./vendor/bin/phpunit --testdox src/tests
