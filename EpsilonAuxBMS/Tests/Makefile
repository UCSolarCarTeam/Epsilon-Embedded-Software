TESTS_DIR = ./tests
TESTS = \
	SetContactorsTaskTest \
	TripTest \
	UpdateChargeAllowanceTaskTest \

SUBCLEAN = $(addsuffix .clean,$(TESTS))

RUN_TEST = $(addsuffix .test,$(TESTS))

default: EpsilonAuxBMSTests test

$(TESTS):
	cd $(TESTS_DIR)/$@ && make -j4
	
EpsilonAuxBMSTests: $(TESTS)

$(SUBCLEAN):
	cd $(TESTS_DIR)/$(basename $@) && make clean

clean: $(SUBCLEAN)

$(RUN_TEST):
	cd $(TESTS_DIR)/$(basename $@) && make test

test: $(RUN_TEST) 