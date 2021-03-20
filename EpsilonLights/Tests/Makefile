TESTS_DIR = ./tests
TESTS = \
	LightsTest \

SUBCLEAN = $(addsuffix .clean,$(TESTS))

RUN_TEST = $(addsuffix .test,$(TESTS))

default: EpsilonLightsTests test

$(TESTS):
	cd $(TESTS_DIR)/$@ && make -j4
	
EpsilonLightsTests: $(TESTS)

$(SUBCLEAN):
	cd $(TESTS_DIR)/$(basename $@) && make clean

clean: $(SUBCLEAN)

$(RUN_TEST):
	cd $(TESTS_DIR)/$(basename $@) && make test

test: $(RUN_TEST) 
