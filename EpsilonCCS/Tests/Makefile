TESTS_DIR = ./tests
TESTS = \
	CCSTest \

SUBCLEAN = $(addsuffix .clean,$(TESTS))

RUN_TEST = $(addsuffix .test,$(TESTS))

default: EpsilonCCSTests test

$(TESTS):
	cd $(TESTS_DIR)/$@ && make -j4
	
EpsilonCCSTests: $(TESTS)

$(SUBCLEAN):
	cd $(TESTS_DIR)/$(basename $@) && make clean

clean: $(SUBCLEAN)

$(RUN_TEST):
	cd $(TESTS_DIR)/$(basename $@) && make test

test: $(RUN_TEST) 