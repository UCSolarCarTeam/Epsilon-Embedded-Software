TESTS_DIR = ./tests
TESTS = \
	StartupTaskTest \
	CommonContactorGatekeeperTaskTest \
	ChargeContactorGatekeeperTaskTest \
	DischargeContactorGatekeeperTaskTest \
	ContactorStatusUpdateTaskTest \
	CheckCurrentTest \

SUBCLEAN = $(addsuffix .clean,$(TESTS))

RUN_TEST = $(addsuffix .test,$(TESTS))

default: EpsilonAuxBmsTests test

$(TESTS):
	cd $(TESTS_DIR)/$@ && make -j4

EpsilonAuxBmsTests: $(TESTS)

$(SUBCLEAN):
	cd $(TESTS_DIR)/$(basename $@) && make clean

clean: $(SUBCLEAN)

$(RUN_TEST):
	cd $(TESTS_DIR)/$(basename $@) && make test

test: $(RUN_TEST)
