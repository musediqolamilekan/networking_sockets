MAKEFILES := Makefile.client Makefile.server 

all: $(MAKEFILES)
	for file in $^ ; do \
		make -f $$file ; \
	done

clean: $(MAKEFILES)
	for file in $^ ; do \
		make -f $$file clean ; \
	done
