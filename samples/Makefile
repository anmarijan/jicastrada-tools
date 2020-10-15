SUBDIRS  := aodplus distcheck freqtnt int2ire ire2int irecomp irecutter irescale iresum nodechg parchg picklines preloadmake rrepair rresum stheader tntintchk tntvolint

.PHONY: all $(SUBDIRS)
all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	find ./ -name "*.o" -exec rm {} \;
