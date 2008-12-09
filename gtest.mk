TEST_PROGS =
GTESTER = gtester

test: all $(TEST_PROGS)
	@test -z "${TEST_PROGS}" || ${GTESTER} --verbose ${TEST_PROGS};
	@ for subdir in $(SUBDIRS) . ; do \
		test "$$subdir" = "." -o "$$subdir" = "po" || \
		(cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $@ ) || exit $? ; \
	done

.PHONY: test
