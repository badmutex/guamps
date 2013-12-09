
subdirs = src
.PHONY:

.PHONY: src
src :
	for dir in ${subdirs}; do \
	  make -C $$dir; \
	done

.PHONY: clean
clean:
	for dir in ${subdirs}; do \
	  make -C $$dir clean; \
	done
